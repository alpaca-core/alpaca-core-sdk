// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalProvider.hpp"
#include "LocalInference.hpp"
#include <ac/Model.hpp>
#include <ac/Instance.hpp>
#include <xec/TaskExecutor.hpp>
#include <xec/ThreadExecution.hpp>
#include <astl/move_capture.hpp>
#include <itlib/shared_from.hpp>
#include <unordered_map>
#include <future>
#include <atomic>

namespace ac {

struct transparent_sting_hash : public std::hash<std::string_view> {
    using hash_type = std::hash<std::string_view>;
    using hash_type::operator();
    using is_transparent = void;
};

#define selfcap self = shared_from(this)

namespace {

std::atomic_uint32_t Instance_OpTaskToken = {};

class LocalInstance : public Instance, public itlib::enable_shared_from {
    std::unique_ptr<LocalInferenceInstance> m_iinstance;
    xec::TaskExecutor& m_executor;
    const xec::TaskExecutor::task_ctoken m_opTaskToken;
public:
    LocalInstance(std::unique_ptr<LocalInferenceInstance> iinstance, xec::TaskExecutor& executor)
        : m_iinstance(astl::move(iinstance))
        , m_executor(executor)
        , m_opTaskToken(1 + Instance_OpTaskToken.fetch_add(1, std::memory_order_relaxed))

    {}

    virtual void runOp(std::string_view op, Dict params, Callback<void, Dict> cb) override {
        m_executor.pushTask([selfcap, op = std::string(op), movecap(params, cb)]() mutable {
            try {
                self->m_iinstance->runOp(op, astl::move(params), [&](Dict result) {
                    cb.progressCb(astl::move(result));
                });
                cb.resultCb({});
            }
            catch (std::exception& ex) {
                cb.resultCb(itlib::unexpected(ac::Error{0, ex.what()}));
                return;
            }
        }, m_opTaskToken);
    }
    virtual void synchronize() override {
        std::promise<void> p;
        auto f = p.get_future();
        m_executor.pushTask([selfcap, movecap(p)]() mutable {
            p.set_value();
        });
        f.wait();
    }
    virtual void initiateAbort(Callback<void> cb) override {
        m_executor.cancelTasksWithToken(m_opTaskToken);
    }
};

class LocalModel : public Model, public itlib::enable_shared_from {
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
                auto instance = self->m_imodel->createInstance(type, astl::move(params));
                assert(instance);
                InstancePtr ptr = std::make_unique<LocalInstance>(astl::move(instance), self->m_executor);
                cb.resultCb(astl::move(ptr));
            }
            catch (std::exception& ex) {
                cb.resultCb(itlib::unexpected(ac::Error{0, ex.what()}));
                return;
            }
        });
    }
};
} // anonymous namespace

class LocalProvider::Impl {
    xec::TaskExecutor m_executor;
    xec::ThreadExecution m_execution;

    std::unordered_map<std::string, LocalInferenceModelLoader*, transparent_sting_hash, std::equal_to<>> m_loaders;
public:
    Impl() : m_execution(m_executor) {
        m_execution.launchThread("ac-inference");
    }

    void addLocalInferenceLoader(std::string_view type, LocalInferenceModelLoader& loader) {
        m_executor.pushTask([this, type = std::string(type), &loader]() mutable {
            m_loaders[astl::move(type)] = &loader;
        });
    }

    void createModel(Dict params, Callback<ModelPtr> cb) {
        m_executor.pushTask([this, movecap(params, cb)]() mutable {
            auto type = params.at("type").get<std::string_view>();
            auto it = m_loaders.find(type);
            if (it == m_loaders.end()) {
                cb.resultCb(itlib::unexpected(ac::Error{0, "Unknown model type"}));
                return;
            }
            auto& loader = *it->second;
            try {
                auto model = loader.loadModel(astl::move(params), [&](float progress) {
                    assert(std::this_thread::get_id() == m_execution.threadId());
                    if (cb.progressCb) {
                        cb.progressCb(progress);
                    }
                });
                assert(model);

                ModelPtr ptr = std::make_unique<LocalModel>(astl::move(model), m_executor);
                cb.resultCb(astl::move(ptr));
            }
            catch (std::exception& ex) {
                cb.resultCb(itlib::unexpected(ac::Error{0, ex.what()}));
                return;
            }
        });
    }
};

LocalProvider::LocalProvider() : m_impl(std::make_unique<Impl>()) {}
LocalProvider::~LocalProvider() = default;

void LocalProvider::createModel(Dict params, Callback<ModelPtr> cb) {
    m_impl->createModel(astl::move(params), astl::move(cb));
}

void LocalProvider::addLocalInferenceLoader(std::string_view type, LocalInferenceModelLoader& loader) {
    m_impl->addLocalInferenceLoader(type, loader);
}

} // namespace ac
