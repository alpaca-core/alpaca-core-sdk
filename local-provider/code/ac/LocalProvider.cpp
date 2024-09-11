// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalProvider.hpp"
#include "LocalInference.hpp"
#include "LocalModelInfo.hpp"
#include "ModelInfo.hpp"
#include "Logging.hpp"
#include <ac/asset/Manager.hpp>
#include <ac/asset/Source.hpp>
#include <ac/Model.hpp>
#include <ac/Instance.hpp>
#include <xec/TaskExecutor.hpp>
#include <xec/ThreadExecution.hpp>
#include <astl/move_capture.hpp>
#include <astl/tsumap.hpp>
#include <astl/coro_lock.hpp>
#include <astl/iile.h>
#include <astl/cow.hpp>
#include <itlib/shared_from.hpp>
#include <itlib/make_ptr.hpp>
#include <unordered_map>
#include <latch>
#include <atomic>
#include <coroutine>

#define LOG_INFO(...) AC_LOCAL_LOG(Info, __VA_ARGS__)

namespace ac {

namespace {

#define selfcap self = shared_from(this)

std::atomic_uint32_t Instance_OpTaskToken = {};

class LocalInstance final : public Instance, public itlib::enable_shared_from {
    ModelPtr m_model; // used to keep the model alive if the user drops the reference
    std::unique_ptr<LocalInferenceInstance> m_iinstance;
    xec::TaskExecutor& m_executor;
    const xec::TaskExecutor::task_ctoken m_opTaskToken;
public:
    LocalInstance(ModelPtr model, std::unique_ptr<LocalInferenceInstance> iinstance, xec::TaskExecutor& executor)
        : m_model(astl::move(model))
        , m_iinstance(astl::move(iinstance))
        , m_executor(executor)
        , m_opTaskToken(1 + Instance_OpTaskToken.fetch_add(1, std::memory_order_relaxed))

    {}

    virtual void runOp(std::string_view op, Dict params, Callback<void, Dict> cb) override {
        m_executor.pushTask([selfcap, op = std::string(op), movecap(params, cb)]() mutable {
            try {
                self->m_iinstance->runOpSync(op, astl::move(params), [&](Dict result) {
                    cb.progressCb({}, astl::move(result));
                });
                cb.resultCb({});
            }
            catch (std::exception& ex) {
                cb.resultCb(itlib::unexpected(ac::Error{ ex.what() }));
                return;
            }
        }, m_opTaskToken);
    }
    virtual void synchronize() override {
        std::latch l(1);
        m_executor.pushTask([&]() mutable {
            l.count_down();
        });
        l.wait();
    }
    virtual void initiateAbort(Callback<void> cb) override {
        m_executor.cancelTasksWithToken(m_opTaskToken);
        m_executor.pushTask([movecap(cb)]() mutable {
            cb.resultCb({});
        });
    }
};

class LocalModel final : public Model, public itlib::enable_shared_from {
    std::unique_ptr<LocalInferenceModel> m_imodel;
    xec::TaskExecutor& m_executor;
public:
    LocalModel(std::unique_ptr<LocalInferenceModel> imodel, xec::TaskExecutor& executor)
        : m_imodel(astl::move(imodel))
        , m_executor(executor)
    {}

    virtual void createInstance(std::string_view type, Dict params, Callback<InstancePtr> cb) override {
        m_executor.pushTask([selfcap, type = std::string(type), movecap(params, cb)]() mutable {
            try {
                auto instance = self->m_imodel->createInstanceSync(type, astl::move(params));

                if (!instance)
                {
                    cb.resultCb(itlib::unexpected(ac::Error{ "Instance couldn't be created!" }));
                    return;
                }

                InstancePtr ptr = std::make_shared<LocalInstance>(self, astl::move(instance), self->m_executor);
                cb.resultCb(astl::move(ptr));
            }
            catch (std::exception& ex) {
                cb.resultCb(itlib::unexpected(ac::Error{ ex.what() }));
                return;
            }
        });
    }
};

// coroutines

class CoTask {
public:
    struct promise_type {
        CoTask get_return_object() noexcept {
            return CoTask{ std::coroutine_handle<promise_type>::from_promise(*this) };
        }
        void unhandled_exception() noexcept { /* terminate? */ }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() noexcept {}
    };
    using Handle = std::coroutine_handle<promise_type>;

    CoTask(CoTask&& other) noexcept : m_handle(std::exchange(other.m_handle, nullptr)) {}
    ~CoTask() {
        if (m_handle) {
            m_handle.destroy();
        }
    }
    Handle take_handle() noexcept {
        return std::exchange(m_handle, nullptr);
    }
private:
    Handle m_handle;
    explicit CoTask(Handle handle) noexcept : m_handle(handle) {}
};

struct async_resume {
    std::coroutine_handle<> handle;

    explicit async_resume(std::coroutine_handle<> h) : handle(h) {}

    async_resume(const async_resume&) = delete;
    async_resume& operator=(const async_resume&) = delete;
    async_resume(async_resume&& other) noexcept : handle(std::exchange(other.handle, nullptr)) {}
    async_resume& operator=(async_resume&& other) noexcept = delete; // don't move around

    ~async_resume() {
        // not resumed, so destoy the handle
        if (handle) {
            handle.destroy();
        }
    }
    void operator()() {
        auto h = std::exchange(handle, nullptr);
        h.resume();
    }
};

class AssetAwaitable {
protected:
    std::string_view m_id;
    asset::AssetManager& m_mgr;
    xec::TaskExecutor& m_executor;
    std::optional<asset::AssetInfo> m_assetInfo;
public:
    AssetAwaitable(std::string_view id, asset::AssetManager& mgr, xec::TaskExecutor& executor)
        : m_id(id)
        , m_mgr(mgr)
        , m_executor(executor)
    {}

    bool await_ready() const noexcept { return false; }
    asset::AssetInfo await_resume() {
        return std::move(*m_assetInfo);
    }
};

class AssetQuery : public AssetAwaitable {
public:
    using AssetAwaitable::AssetAwaitable;

    void await_suspend(std::coroutine_handle<> handle) {
        m_mgr.queryAsset(std::string(m_id), [this, handle](std::string_view, const asset::AssetInfo& data) {
            m_assetInfo = data;
            m_executor.pushTask(async_resume(handle));
        });
    }
};

class AssetGet : public AssetAwaitable {
    asset::AssetManager::GetAssetProgressCb m_progressCb;
public:
    AssetGet(std::string_view id, asset::AssetManager& mgr, xec::TaskExecutor& executor, asset::AssetManager::GetAssetProgressCb progressCb)
        : AssetAwaitable(id, mgr, executor)
        , m_progressCb(astl::move(progressCb))
    {}

    void await_suspend(std::coroutine_handle<> handle) {
        m_mgr.getAsset(std::string(m_id),
            [this, handle](std::string_view, const asset::AssetInfo& data) {
                m_assetInfo = data;
                m_executor.pushTask(async_resume(handle));
            },
            astl::move(m_progressCb)
        );
    }
};

} // anonymous namespace

class LocalProvider::Impl {
    astl::tsumap<LocalInferenceModelLoader*> m_loaders;

    struct ModelManifestEntry {
        // this is a quite complex struct
        // it contains the model info which is to be shared with the outside world upon model queries
        // the info is also used across the provider (in a single strand) to manage model creation and assets
        // some of the provider's methods are coroutines which get spliced into the strand
        //
        // 1. to facilitate sharing between threads we use the CoW object
        // 2. to facilitate sharing between coroutines we use the coro_lock (thus coroutines wait before touching infos)

        astl::coro_lock lock;
        astl::sp_cow<LocalModelInfo> info;
    };

    // the complexity just grows
    // the model manifest is a map of model ids to model infos (btw could be made into an unordered_set)
    // since spliced coroutines lock entries, we should never remove them from the map, otherwise we could
    // leave awaiters hanging with an invalid reference
    // and thus we created the spec that model infos are permanent, yay :)
    // note that they are not immutable, model sources could still change them, just never remove them
    astl::tsumap<std::unique_ptr<ModelManifestEntry>> m_modelManifest;

    // keep the position order of the next items:
    // * they must be the last (first to be destroyed)
    // * the asset manager must be destroyed first
    xec::TaskExecutor m_executor;
    xec::ThreadExecution m_execution;
    asset::AssetManager m_assetMgr;
public:
    Impl() : m_execution(m_executor) {
        m_execution.launchThread("ac-inference");
    }

    ~Impl() {
        // complex shutdown logic since we're running threads and communicating by raw refs :)

        // first shut down the local execution and thus stop any tasks issued to m_assetMgr
        m_execution.stopAndJoinThread();

        // then m_assetMgr being the last member will be destroyed and shut down first so it, in turn,
        // stops issuing tasks to our executor

        // any hanging tasks are just discarded
    }

    void addAssetSource(std::unique_ptr<asset::AssetSource> source, int priority) {
        // asset manager is thread safe, so no need to push this to the executor
        m_assetMgr.addSource(astl::move(source), priority);
    }

    void addLocalInferenceLoader(std::string_view type, LocalInferenceModelLoader& loader) {
        m_executor.pushTask([this, type = std::string(type), &loader]() mutable {
            m_loaders[astl::move(type)] = &loader;
        });
    }

    void co_splice(std::coroutine_handle<> h) {
        m_executor.pushTask(async_resume(h));
    }

    void co_splice(CoTask task) {
        co_splice(task.take_handle());
    }

    CoTask coAddModel(ModelInfo info) {
        // spliced coroutine: prepare lock
        astl::coro_lock::guard lockGuard;

        LOG_INFO("adding model: ", info.id);

        auto iter = m_modelManifest.find(info.id);
        if (iter == m_modelManifest.end()) {
            // creating a new model info
            // update the manifest so that everyone knows it's there and immediately lock it
            auto entry = std::make_unique<ModelManifestEntry>();
            lockGuard = entry->lock.try_lock_guard(); // no need to await since it's just created
            assert(lockGuard); // must be able to lock immediately
            iter = m_modelManifest.emplace(info.id, astl::move(entry)).first;
        }
        else {
            // we're updating the model info so just lock it
            lockGuard = co_await iter->second->lock;
        }

        auto& localInfo = iter->second->info;
        static_cast<ModelInfo&>(localInfo.w()) = std::move(info); // slice relevant part
        localInfo.w().localAssets.reserve(localInfo->assets.size());

        // since we can detach in a spliced coroutine, we can't use a range-based for loop here
        for (size_t i = 0; i < localInfo->assets.size(); ++i) {
            auto& asset = localInfo->assets[i];
            LOG_INFO("querying asset: ", asset.id);
            localInfo.w().localAssets.push_back(co_await AssetQuery(asset.id, m_assetMgr, m_executor));
        }
    }

    void addModel(ModelInfo info) {
        co_splice(coAddModel(astl::move(info)));
    }

    CoTask coCreateModel(std::string id, Dict params, Callback<ModelPtr> cb) {
        try {
            auto f = m_modelManifest.find(id);
            if (f == m_modelManifest.end()) {
                cb.resultCb(itlib::unexpected(ac::Error{"Unknown model id"}));
                co_return;
            }
            auto infoLock = co_await f->second->lock;
            auto& info = f->second->info;
            auto& type = info->inferenceType;

            auto it = m_loaders.find(type);
            if (it == m_loaders.end()) {
                cb.resultCb(itlib::unexpected(ac::Error{"Unknown model type"}));
                co_return;
            }
            auto& loader = *it->second;

            // find assets which are not loaded and don't have an error
            // (we won't try to load assets with errors)
            auto gettable = [&](const asset::AssetInfo& a) {
                return !a.path && !a.error;
            };
            auto hasAssetsToLoad = iile([&] {
                for (auto& a : info->localAssets) {
                    if (!a.path && !a.error) {
                        return true;
                    }
                }
                return false;
            });
            if (hasAssetsToLoad) {
                auto localAssets = info->localAssets;
                assert(localAssets.size() == info->assets.size());
                for (size_t i = 0; i < localAssets.size(); ++i) {
                    auto& asset = localAssets[i];
                    if (!gettable(asset)) continue;

                    auto& aid = info->assets[i].id;
                    LOG_INFO("getting asset: ", aid);
                    asset = co_await AssetGet(aid, m_assetMgr, m_executor, [&](std::string_view, float p) {
                        cb.progressCb(aid, p);
                    });
                }
                info.w().localAssets = astl::move(localAssets);
            }

            LOG_INFO("loading model: ", id);
            auto model = loader.loadModelSync(info.detach(), astl::move(params), [&](float progress) {
                assert(std::this_thread::get_id() == m_execution.threadId());
                if (cb.progressCb) {
                    cb.progressCb(id, progress);
                }
            });

            if (!model) {
                cb.resultCb(itlib::unexpected(ac::Error{"Model couldn't be loaded!"}));
                co_return;
            }

            ModelPtr ptr = std::make_shared<LocalModel>(astl::move(model), m_executor);
            cb.resultCb(astl::move(ptr));
        }
        catch (std::exception& ex) {
            cb.resultCb(itlib::unexpected(ac::Error{ex.what()}));
        }
    }

    void createModel(std::string_view id, Dict params, Callback<ModelPtr> cb) {
        co_splice(coCreateModel(std::string(id), astl::move(params), astl::move(cb)));
    }
};

LocalProvider::LocalProvider() : m_impl(std::make_unique<Impl>()) {}
LocalProvider::~LocalProvider() = default;

void LocalProvider::addModel(ModelInfo info) {
    m_impl->addModel(astl::move(info));
}

void LocalProvider::createModel(std::string_view id, Dict params, Callback<ModelPtr> cb) {
    m_impl->createModel(id, astl::move(params), astl::move(cb));
}

void LocalProvider::addLocalInferenceLoader(std::string_view type, LocalInferenceModelLoader& loader) {
    m_impl->addLocalInferenceLoader(type, loader);
}

void LocalProvider::addAssetSource(std::unique_ptr<asset::AssetSource> source, int priority) {
    m_impl->addAssetSource(astl::move(source), priority);
}

} // namespace ac
