// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalProvider.hpp"
#include "LocalInference.hpp"
#include "LocalModelInfo.hpp"
#include "ModelInfo.hpp"
#include <ac/Model.hpp>
#include <ac/Instance.hpp>
#include <xec/TaskExecutor.hpp>
#include <xec/ThreadExecution.hpp>
#include <astl/move_capture.hpp>
#include <astl/tsumap.hpp>
#include <itlib/shared_from.hpp>
#include <itlib/make_ptr.hpp>
#include <unordered_map>
#include <latch>
#include <atomic>

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
                    cb.progressCb(astl::move(result));
                });
                cb.resultCb({});
            }
            catch (std::exception& ex) {
                cb.resultCb(itlib::unexpected(ac::Error{ex.what()}));
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
                    cb.resultCb(itlib::unexpected(ac::Error{"Instance couldn't be created!"}));
                    return;
                }

                InstancePtr ptr = std::make_shared<LocalInstance>(self, astl::move(instance), self->m_executor);
                cb.resultCb(astl::move(ptr));
            }
            catch (std::exception& ex) {
                cb.resultCb(itlib::unexpected(ac::Error{ex.what()}));
                return;
            }
        });
    }
};
} // anonymous namespace

class LocalProvider::Impl {
    astl::tsumap<LocalInferenceModelLoader*> m_loaders;
    astl::tsumap<std::shared_ptr<LocalModelInfo>> m_modelManifest; // could be made into an unordered_set

    // these must the last members (first to be destroyed)
    // if there are pending tasks, they will be finalized here and they may access other members
    xec::TaskExecutor m_executor;
    xec::ThreadExecution m_execution;
public:
    Impl() : m_execution(m_executor) {
        m_execution.launchThread("ac-inference");
    }

    void addLocalInferenceLoader(std::string_view type, LocalInferenceModelLoader& loader) {
        m_executor.pushTask([this, type = std::string(type), &loader]() mutable {
            m_loaders[astl::move(type)] = &loader;
        });
    }

    void addModel(ModelInfo info) {
        m_executor.pushTask([this, movecap(info)]() mutable {
            auto localInfo = itlib::make_shared(LocalModelInfo{astl::move(info)});
            localInfo->localAssets.resize(localInfo->assets.size());
            for (size_t i = 0; i < localInfo->assets.size(); ++i) {
                // temporary, until we integrate asset manager properly
                localInfo->localAssets[i].path = localInfo->assets[i].id;
            }
            m_modelManifest[localInfo->id] = localInfo;
        });
    }

    void createModel(std::string_view id, Dict params, Callback<ModelPtr> cb) {
        m_executor.pushTask([this, id = std::string(id), movecap(params, cb)]() mutable {
            try {
                auto f = m_modelManifest.find(id);
                if (f == m_modelManifest.end()) {
                    cb.resultCb(itlib::unexpected(ac::Error{"Unknown model id"}));
                    return;
                }
                auto info = LocalModelInfoPtr(f->second);
                auto& type = info->inferenceType;

                auto it = m_loaders.find(type);
                if (it == m_loaders.end()) {
                    cb.resultCb(itlib::unexpected(ac::Error{"Unknown model type"}));
                    return;
                }
                auto& loader = *it->second;

                auto model = loader.loadModelSync(astl::move(info), astl::move(params), [&](float progress) {
                    assert(std::this_thread::get_id() == m_execution.threadId());
                    if (cb.progressCb) {
                        cb.progressCb(progress);
                    }
                });

                if (!model)
                {
                    cb.resultCb(itlib::unexpected(ac::Error{"Model couldn't be loaded!"}));
                    return;
                }

                ModelPtr ptr = std::make_shared<LocalModel>(astl::move(model), m_executor);
                cb.resultCb(astl::move(ptr));
            }
            catch (std::exception& ex) {
                cb.resultCb(itlib::unexpected(ac::Error{ex.what()}));
                return;
            }
        });
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

} // namespace ac
