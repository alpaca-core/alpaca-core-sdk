// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalProvider.hpp"
#include "LocalInference.hpp"
#include "Logging.hpp"
#include <ac/Model.hpp>
#include <ac/Instance.hpp>
#include <xec/TaskExecutor.hpp>
#include <xec/ThreadExecution.hpp>
#include <xec/ThreadName.hpp>
#include <astl/move_capture.hpp>
#include <astl/tsumap.hpp>
#include <itlib/shared_from.hpp>
#include <itlib/make_ptr.hpp>
#include <unordered_map>
#include <latch>
#include <atomic>

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

    virtual void runOp(std::string_view op, Dict params, OpCallback cb) override {
        m_executor.pushTask([selfcap, op = std::string(op), movecap(params, cb)]() mutable {
            try {
                self->m_iinstance->runOpSync(op, astl::move(params), astl::move(cb.streamCb), astl::move(cb.progressCb));
                cb.completionCb({});
            }
            catch (std::exception& ex) {
                cb.completionCb(itlib::unexpected(ac::Error{ex.what()}));
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
    virtual void initiateAbort(BasicCb<void> cb) override {
        m_executor.cancelTasksWithToken(m_opTaskToken);
        m_executor.pushTask([movecap(cb)]() mutable {
            cb();
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

    virtual void createInstance(std::string_view type, Dict params, ResultCb<InstancePtr> cb) override {
        m_executor.pushTask([selfcap, type = std::string(type), movecap(params, cb)]() mutable {
            try {
                auto instance = self->m_imodel->createInstanceSync(type, astl::move(params));

                if (!instance)
                {
                    cb(itlib::unexpected(ac::Error{ "Instance couldn't be created!" }));
                    return;
                }

                InstancePtr ptr = std::make_shared<LocalInstance>(self, astl::move(instance), self->m_executor);
                cb(astl::move(ptr));
            }
            catch (std::exception& ex) {
                cb(itlib::unexpected(ac::Error{ ex.what() }));
                return;
            }
        });
    }
};
} // anonymous namespace

class LocalProvider::Impl {
    astl::tsumap<LocalInferenceModelLoader*> m_loaders;

    // inference
    xec::TaskExecutor m_executor;
    xec::LocalExecution m_execution;
    std::thread m_thread;
public:
    Impl(uint32_t flags)
        : m_execution(m_executor)
    {
        if (!(flags & No_LaunchThread)) {
            launchThread();
        }
    }

    ~Impl() {
        // now we could always call abortRun() here, but that has the potential to hide misuse
        // so to increase the likelihood of a crash, we do it only if we own the execution
        if (m_thread.joinable()) {
            abortRun();
            m_thread.join();
        }
    }

    void launchThread() {
        m_thread = std::thread([this]() {
            xec::SetThisThreadName("ac-local");
            run();
        });
    }

    void addLocalInferenceLoader(std::string_view type, LocalInferenceModelLoader& loader) {
        m_executor.pushTask([this, type = std::string(type), &loader]() mutable {
            m_loaders[astl::move(type)] = &loader;
        });
    }

    void createModel(ModelDesc desc, Dict params, ModelCb cb) {
        m_executor.pushTask([this, movecap(desc, params, cb)]() mutable {
            try {
                auto it = m_loaders.find(desc.inferenceType);
                if (it == m_loaders.end()) {
                    cb.resultCb(itlib::unexpected(ac::Error{ "Unknown model type" }));
                    return;
                }
                auto& loader = *it->second;

                LOG_INFO("loading model: ", desc.name);
                auto model = loader.loadModelSync(astl::move(desc), astl::move(params), astl::move(cb.progressCb));

                if (!model) {
                    cb.resultCb(itlib::unexpected(ac::Error{ "Model couldn't be loaded!" }));
                    return;
                }

                ModelPtr ptr = std::make_shared<LocalModel>(astl::move(model), m_executor);
                cb.resultCb(astl::move(ptr));
            }
            catch (std::exception& ex) {
                cb.resultCb(itlib::unexpected(ac::Error{ex.what()}));
            }
        });
    }

    void abortRun() {
        m_executor.stop();
    }
    void pushStop() {
        m_executor.pushTask([this]() {
            m_executor.stop();
        });
    }
    void run() {
        m_execution.run();
    }
};

LocalProvider::LocalProvider(uint32_t flags) : m_impl(std::make_unique<Impl>(flags)) {}
LocalProvider::~LocalProvider() = default;

void LocalProvider::createModel(ModelDesc desc, Dict params, ModelCb cb) {
    m_impl->createModel(astl::move(desc), astl::move(params), astl::move(cb));
}

void LocalProvider::addLocalInferenceLoader(std::string_view type, LocalInferenceModelLoader& loader) {
    m_impl->addLocalInferenceLoader(type, loader);
}

void LocalProvider::run() {
    m_impl->run();
}

void LocalProvider::abortRun() {
    m_impl->abortRun();
}

void LocalProvider::pushStop() {
    m_impl->pushStop();
}

} // namespace ac
