// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalDummy.hpp"
#include "DummyProviderSchema.hpp"

#include "Instance.hpp"
#include "Model.hpp"

#include "aclp-dummy-version.h"

#include <ac/local/Instance.hpp>
#include <ac/local/Model.hpp>
#include <ac/local/Provider.hpp>

#include <ac/schema/DispatchHelpers.hpp>

#include <ac/SessionHandler.hpp>

#include <astl/move.hpp>
#include <astl/iile.h>
#include <astl/throw_stdex.hpp>
#include <astl/workarounds.h>

#include <deque>

namespace ac::local {

namespace {

template <typename Fsm>
class TFsm {
protected:
    TFsm() = default;
    ~TFsm() = default;
    TFsm(const TFsm&) = delete;
    TFsm& operator=(const TFsm&) = delete;

    struct State {
        friend Fsm;
        Fsm& fsm;

        State(Fsm* fsm) : fsm(*fsm) {}
        State(const State&) = delete;
        State& operator=(const State&) = delete;

        virtual void on(Frame& t) = 0;
    protected:
        ~State() = default;
    };

    State* m_state = nullptr;

    template <typename NextState, typename... Args>
    void transitionTo(NextState& ns, Args&&... args) {
        m_state = &ns;
        ns.enter(std::forward<Args>(args)...);
    }
public:
    void on(Frame& t) {
        m_state->on(t);
    }
};

class DummySessionHandler final : public TFsm<DummySessionHandler>, public SessionHandler {
    struct StateInitial final : public State {
        using State::State;

        using Schema = schema::DummyProvider;

        static dummy::Model::Params ModelParams_fromDict(Dict& d) {
            auto schemaParams = schema::Struct_fromDict<Schema::Params>(std::move(d));
            dummy::Model::Params ret;
            ret.path = schemaParams.filePath.valueOr("");
            ret.splice = astl::move(schemaParams.spliceString.valueOr(""));
            return ret;
        }

        void enter() {}
        void on(Frame& t) override {
            if (t.op != "load") {
                throw_ex{} << "dummy: expected 'load' op, got: " << t.op;
            }
            // btodo: abort
            auto params = ModelParams_fromDict(t.data);
            fsm.transitionTo(fsm.model, std::make_unique<dummy::Model>(params));
        }
    };
    struct StateModel final : public State {
        using Schema = schema::DummyProvider;
        std::unique_ptr<dummy::Model> m_model;

        static dummy::Instance::InitParams InitParams_fromDict(Dict&& d) {
            auto schemaParams = schema::Struct_fromDict<Schema::InstanceGeneral::Params>(astl::move(d));
            dummy::Instance::InitParams ret;
            ret.cutoff = schemaParams.cutoff;
            return ret;
        }

        using State::State;
        void enter(std::unique_ptr<dummy::Model> model) {
            m_model = std::move(model);
        }
        void on(Frame& t) override {
            if (t.op != "create") {
                throw_ex{} << "dummy: expected 'create' op, got: " << t.op;
            }
            auto params = InitParams_fromDict(astl::move(t.data));
            fsm.transitionTo(fsm.instance, std::make_unique<dummy::Instance>(*m_model, astl::move(params)));
        }
    };
    struct StateInstance final : public State {
        std::unique_ptr<dummy::Instance> m_instance;

        using Schema = schema::DummyProvider::InstanceGeneral;
        schema::OpDispatcherData m_dispatcherData;

        using State::State;
        void enter(std::unique_ptr<dummy::Instance> instance) {
            m_instance = std::move(instance);
            schema::registerHandlers<schema::DummyInterface::Ops>(m_dispatcherData, *this);
        }
        void on(Frame& f) override {
            auto ret = m_dispatcherData.dispatch(f.op, std::move(f.data));
            if (!ret) {
                throw_ex{} << "dummy: unknown op: " << f.op;
            }
            fsm.writeFrame(Frame{f.op, *ret});
        }

        schema::DummyInterface::OpRun::Return on(schema::DummyInterface::OpRun, schema::DummyInterface::OpRun::Params params) {
            dummy::Instance::SessionParams sparams;
            sparams.splice = params.splice;
            sparams.throwOn = params.throwOn;

            auto s = m_instance->newSession(std::move(params.input), sparams);

            schema::DummyInterface::OpRun::Return ret;
            auto& res = ret.result.materialize();
            for (auto& w : s) {
                res += w;
                res += ' ';
            }
            if (!res.empty()) {
                // remove last space
                res.pop_back();
            }

            return ret;
        }
    };

    StateInitial initial{this};
    StateModel model{this};
    StateInstance instance{this};

    std::deque<Frame> m_outQueue;

    void pumpInFrames() {
        while (true) {
            auto f = getSessionInFrame();
            if (!f) break;
            try {
                on(*f);
            }
            catch (std::exception& e) {
                writeFrame(Frame{"error", e.what()});
            }
        }
        pollSessionInFramesAvailable();
    }
    void writeFrame(Frame&& t) {
        if (sessionAcceptsOutFrames()) {
            pushSessionOutFrame(astl::move(t));
            return;
        }
        m_outQueue.push_back(std::move(t));
    }
    void dumpOutQueue() {
        while (!m_outQueue.empty() && sessionAcceptsOutFrames()) {
            pushSessionOutFrame(std::move(m_outQueue.front()));
            m_outQueue.pop_front();
        }
        pollSessionOutFramesAccepted();
    }

    virtual void shAttached(const SessionHandlerPtr&) override {
        transitionTo(initial);
        pumpInFrames();
    }
    virtual void shSessionClosed() override {
        // for sanity force a crash if touched again
        m_state = nullptr;
    }
    virtual void shOnAvailableSessionInFrames() override {
        pumpInFrames();
    }
    virtual void shOnSessionAcceptsOutFrames() override {
        dumpOutQueue();
    }
};

class DummyInstance final : public Instance {
    std::shared_ptr<dummy::Model> m_model;
    dummy::Instance m_instance;
    schema::OpDispatcherData m_dispatcherData;
public:
    using Schema = schema::DummyProvider::InstanceGeneral;

    static dummy::Instance::InitParams InitParams_fromDict(Dict&& d) {
        auto schemaParams = schema::Struct_fromDict<Schema::Params>(astl::move(d));
        dummy::Instance::InitParams ret;
        ret.cutoff = schemaParams.cutoff;
        return ret;
    }

    DummyInstance(std::shared_ptr<dummy::Model> model, Dict&& params)
        : m_model(astl::move(model))
        , m_instance(*m_model, InitParams_fromDict(astl::move(params)))
    {
        schema::registerHandlers<schema::DummyInterface::Ops>(m_dispatcherData, *this);
    }

    schema::DummyInterface::OpRun::Return on(schema::DummyInterface::OpRun, schema::DummyInterface::OpRun::Params params) {
        dummy::Instance::SessionParams sparams;
        sparams.splice = params.splice;
        sparams.throwOn = params.throwOn;

        auto s = m_instance.newSession(std::move(params.input), sparams);

        schema::DummyInterface::OpRun::Return ret;
        auto& res = ret.result.materialize();
        for (auto& w : s) {
            res += w;
            res += ' ';
        }
        if (!res.empty()) {
            // remove last space
            res.pop_back();
        }

        return ret;
    }

    virtual Dict runOp(std::string_view op, Dict params, ProgressCb) override {
        auto ret = m_dispatcherData.dispatch(op, astl::move(params));
        if (!ret) {
            throw_ex{} << "dummy: unknown op: " << op;
        }
        return *ret;
    }
};

class DummyModel final : public Model {
    std::shared_ptr<dummy::Model> m_model;
public:
    using Schema = schema::DummyProvider;

    static dummy::Model::Params ModelParams_fromDict(Dict& d) {
        auto schemaParams = schema::Struct_fromDict<Schema::Params>(std::move(d));
        dummy::Model::Params ret;
        ret.path = schemaParams.filePath.valueOr("");
        ret.splice = astl::move(schemaParams.spliceString.valueOr(""));
        return ret;
    }

    DummyModel(Dict& params)
        : m_model(std::make_shared<dummy::Model>(ModelParams_fromDict(params)))
    {}

    virtual std::unique_ptr<Instance> createInstance(std::string_view type, Dict params) override {
        if (type == "general") {
            return std::make_unique<DummyInstance>(m_model, astl::move(params));
        }
        else {
            throw_ex{} << "dummy: unknown instance type: " << type;
            MSVC_WO_10766806();
        }
    }
};

class DummyProvider final : public Provider {
public:
    virtual const Info& info() const noexcept override {
        static Info i = {
            .name = "ac-local dummy",
            .vendor = "Alpaca Core",
        };
        return i;
    }

    virtual bool canLoadModel(const ModelAssetDesc& desc, const Dict&) const noexcept override {
        return desc.type == "dummy";
    }

    virtual ModelPtr loadModel(ModelAssetDesc desc, Dict params, ProgressCb pcb) override {
        if (desc.assets.size() > 1) throw_ex{} << "dummy: expected one or zero assets";

        if (desc.assets.empty()) {
            // synthetic model
            if (pcb) {
                if (!pcb("synthetic", 0.5f)) {
                    throw_ex{} << "dummy: loading model aborted";
                }
            }
            return std::make_shared<DummyModel>(params);
        }
        else {
            auto& fname = desc.assets.front().path;
            params["file_path"] = fname;
            if (pcb) {
                if (!pcb(fname, 0.1f)) {
                    throw_ex{} << "dummy: loading model aborted";
                }
            }
            return std::make_shared<DummyModel>(params);
        }
    }

    virtual SessionHandlerPtr createSessionHandler(std::string_view) {
        return std::make_shared<DummySessionHandler>();
    }
};

} // namespace

} // namespace ac::local

namespace ac::dummy {

std::vector<ac::local::ProviderPtr> getProviders() {
    std::vector<ac::local::ProviderPtr> ret;
    ret.push_back(std::make_unique<local::DummyProvider>());
    return ret;
}

local::PluginInterface getPluginInterface() {
    return {
        .label = "ac-local dummy",
        .desc = "Dummy plugin for ac-local",
        .vendor = "Alpaca Core",
        .version = astl::version{
            ACLP_dummy_VERSION_MAJOR, ACLP_dummy_VERSION_MINOR, ACLP_dummy_VERSION_PATCH
        },
        .init = nullptr,
        .getProviders = getProviders,
    };
}

} // namespace ac::dummy
