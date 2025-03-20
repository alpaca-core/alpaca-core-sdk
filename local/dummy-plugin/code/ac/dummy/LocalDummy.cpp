// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalDummy.hpp"
#include "DummyInterface.hpp"
#include "Logging.hpp"

#include "Instance.hpp"
#include "Model.hpp"

#include "aclp-dummy-version.h"

#include <ac/local/Service.hpp>
#include <ac/local/ServiceFactory.hpp>
#include <ac/local/ServiceInfo.hpp>
#include <ac/local/Backend.hpp>
#include <ac/local/ResourceManager.hpp>

#include <ac/schema/OpDispatchHelpers.hpp>
#include <ac/schema/FrameHelpers.hpp>

#include <ac/FrameUtil.hpp>
#include <ac/frameio/IoEndpoint.hpp>

#include <ac/xec/coro.hpp>
#include <ac/io/exception.hpp>

#include <astl/move.hpp>
#include <astl/throw_stdex.hpp>

#include <deque>

namespace ac::local {

namespace {

namespace sc = schema::dummy;

using namespace ac::frameio;

struct BasicRunner {
    schema::OpDispatcherData m_dispatcherData;

    Frame dispatch(Frame& f) {
        try {
            auto ret = m_dispatcherData.dispatch(f.op, std::move(f.data));
            if (!ret) {
                throw_ex{} << "dummy: unknown op: " << f.op;
            }
            return {f.op, *ret};
        }
        catch (io::stream_closed_error&) {
            throw;
        }
        catch (std::exception& e) {
            return {"error", e.what()};
        }
    }
};

xec::coro<void> Dummy_runStream(IoEndpoint& io, astl::generator<const std::string&> s) {
    using Schema = sc::StateStreaming;
    co_await io.push(Frame_stateChange(Schema::id));
    Frame abortFrame;
    for (auto& w : s) {
        if (io.get(abortFrame).success()) {
            if (abortFrame.op == Schema::OpAbort::id) {
                co_return;
            }
            else {
                DUMMY_LOG(Warning, "Unexpected frame: ", abortFrame.op);
            }
        }

        co_await io.push(Frame_fromStreamType(Schema::StreamToken{}, w));
    }
}

xec::coro<void> Dummy_runInstance(IoEndpoint& io, std::unique_ptr<dummy::Instance> instance) {
    using Schema = sc::StateInstance;

    struct Runner : public BasicRunner {
        IoEndpoint& m_io;
        dummy::Instance& m_instance;

        explicit Runner(IoEndpoint& io, dummy::Instance& instance)
            : m_io(io)
            , m_instance(instance)

        {
            schema::registerHandlers<Schema::Ops>(m_dispatcherData, *this);
        }

        xec::coro<void> nextState;

        astl::generator<const std::string&> createSession(Schema::StateInstance::InferenceParams& params) {
            dummy::Instance::SessionParams sparams;
            sparams.splice = params.splice;
            sparams.throwOn = params.throwOn;

            return m_instance.newSession(std::move(params.input), sparams);
        }

        Schema::OpRun::Return on(Schema::OpRun, Schema::OpRun::Params params) {
            auto s = createSession(params);

            Schema::OpRun::Return ret;
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

        Schema::OpStream::Return on(Schema::OpStream, Schema::OpStream::Params params) {
            nextState = Dummy_runStream(m_io, createSession(params));
            return {};
        }
    };

    co_await io.push(Frame_stateChange(Schema::id));

    Runner runner(io, *instance);

    while (true) {
        auto f = co_await io.poll();
        co_await io.push(runner.dispatch(f.value));
        if (runner.nextState) {
            co_await runner.nextState;
            co_await io.push(Frame_stateChange(Schema::id));
        }
    }
}

xec::coro<void> Dummy_runModel(IoEndpoint& io, dummy::Model& model) {
    using Schema = sc::StateModelLoaded;

    struct Runner : public BasicRunner {
        dummy::Model& model;

        explicit Runner(dummy::Model& m) : model(m) {
            schema::registerHandlers<Schema::Ops>(m_dispatcherData, *this);
        }

        std::unique_ptr<dummy::Instance> instance;

        static dummy::Instance::InitParams InitParams_fromSchema(sc::StateModelLoaded::OpCreateInstance::Params schemaParams) {
            dummy::Instance::InitParams ret;
            ret.cutoff = schemaParams.cutoff;
            return ret;
        }

        Schema::OpCreateInstance::Return on(Schema::OpCreateInstance, Schema::OpCreateInstance::Params params) {
            instance = std::make_unique<dummy::Instance>(model, InitParams_fromSchema(params));
            return {};
        }
    };

    co_await io.push(Frame_stateChange(Schema::id));

    Runner runner(model);

    while (true) {
        auto f = co_await io.poll();
        co_await io.push(runner.dispatch(f.value));
        if (runner.instance) {
            co_await Dummy_runInstance(io, astl::move(runner.instance));
        }
    }
}

struct DummyModelResource : public dummy::Model, public Resource{
    using dummy::Model::Model;

    using Manager = ResourceManager<dummy::Model::Params, DummyModelResource>;
    using Lock = ResourceLock<DummyModelResource>;
};

xec::coro<void> Dummy_runSession(StreamEndpoint ep, DummyModelResource::Manager& rm) {
    using Schema = sc::StateInitial;

    struct Runner : public BasicRunner {
        Runner(DummyModelResource::Manager& rm)
            : m_resourceManager(rm)
        {
            schema::registerHandlers<Schema::Ops>(m_dispatcherData, *this);
        }

        DummyModelResource::Manager& m_resourceManager;

        DummyModelResource::Lock model;

        static dummy::Model::Params ModelParams_fromSchema(sc::StateInitial::OpLoadModel::Params schemaParams) {
            dummy::Model::Params ret;
            ret.path = schemaParams.filePath.valueOr("");
            ret.splice = astl::move(schemaParams.spliceString.valueOr(""));
            return ret;
        }

        Schema::OpLoadModel::Return on(Schema::OpLoadModel, Schema::OpLoadModel::Params sparams) {
            auto mparams = ModelParams_fromSchema(sparams);
            model = m_resourceManager.find(mparams);

            if (!model) {
                model = m_resourceManager.add(
                    mparams,
                    std::make_shared<DummyModelResource>(mparams)
                );
            }

            return {};
        }
    };

    try {
        auto ex = co_await xec::executor{};
        IoEndpoint io(std::move(ep), ex);

        co_await io.push(Frame_stateChange(Schema::id));

        Runner runner(rm);

        while (true) {
            auto f = co_await io.poll();
            co_await io.push(runner.dispatch(f.value));
            if (runner.model) {
                co_await Dummy_runModel(io, *runner.model);
            }
        }
    }
    catch (io::stream_closed_error&) {
        co_return;
    }
}

ServiceInfo g_serviceInfo = {
    .name = "ac-local dummy",
    .vendor = "Alpaca Core",
};

struct DummyService final : public Service {
    xec::strand cpuStrand;

    DummyModelResource::Manager m_resourceManager;

    virtual const ServiceInfo& info() const noexcept override {
        return g_serviceInfo;
    }

    virtual void createSession(frameio::StreamEndpoint ep, std::string_view) override {
        co_spawn(cpuStrand, Dummy_runSession(std::move(ep), m_resourceManager));
    }
};

struct DummyServiceFactory final : public ServiceFactory {
    virtual const ServiceInfo& info() const noexcept override {
        return g_serviceInfo;
    }
    virtual std::unique_ptr<Service> createService(const Backend& backend) const override {
        auto svc = std::make_unique<DummyService>();
        svc->cpuStrand = backend.xctx().cpu;
        return svc;
    }
};

} // namespace

} // namespace ac::local

namespace ac::dummy {

std::vector<const local::ServiceFactory*> getFactories() {
    static local::DummyServiceFactory factory;
    return {&factory};
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
        .getServiceFactories = getFactories,
    };
}

} // namespace ac::dummy
