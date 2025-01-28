// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalDummy.hpp"
#include "DummyInterface.hpp"

#include "Instance.hpp"
#include "Model.hpp"

#include "aclp-dummy-version.h"

#include <ac/local/Provider.hpp>

#include <ac/schema/OpDispatchHelpers.hpp>

#include <ac/frameio/SessionCoro.hpp>
#include <ac/FrameUtil.hpp>

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
        catch (coro::IoClosed&) {
            throw;
        }
        catch (std::exception& e) {
            return {"error", e.what()};
        }
    }
};

SessionCoro<void> Dummy_runInstance(coro::Io io, std::unique_ptr<dummy::Instance> instance) {
    using Schema = sc::StateInstance;

    struct Runner : public BasicRunner {
        dummy::Instance& m_instance;

        explicit Runner(dummy::Instance& instance) : m_instance(instance) {
            schema::registerHandlers<Schema::Ops>(m_dispatcherData, *this);
        }
        Schema::OpRun::Return on(Schema::OpRun, Schema::OpRun::Params params) {
            dummy::Instance::SessionParams sparams;
            sparams.splice = params.splice;
            sparams.throwOn = params.throwOn;

            auto s = m_instance.newSession(std::move(params.input), sparams);

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
    };

    co_await io.pushFrame(Frame_stateChange(Schema::id));

    Runner runner(*instance);

    while (true) {
        auto f = co_await io.pollFrame();
        co_await io.pushFrame(runner.dispatch(f.frame));
    }
}

SessionCoro<void> Dummy_runModel(coro::Io io, std::unique_ptr<dummy::Model> model) {
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

    co_await io.pushFrame(Frame_stateChange(Schema::id));

    Runner runner(*model);

    while (true) {
        auto f = co_await io.pollFrame();
        co_await io.pushFrame(runner.dispatch(f.frame));
        if (runner.instance) {
            co_await Dummy_runInstance(io, astl::move(runner.instance));
        }
    }
}

SessionCoro<void> Dummy_runSession() {
    using Schema = sc::StateInitial;

    struct Runner : public BasicRunner {
        Runner() {
            schema::registerHandlers<Schema::Ops>(m_dispatcherData, *this);
        }

        std::unique_ptr<dummy::Model> model;

        static dummy::Model::Params ModelParams_fromSchema(sc::StateInitial::OpLoadModel::Params schemaParams) {
            dummy::Model::Params ret;
            ret.path = schemaParams.filePath.valueOr("");
            ret.splice = astl::move(schemaParams.spliceString.valueOr(""));
            return ret;
        }

        Schema::OpLoadModel::Return on(Schema::OpLoadModel, Schema::OpLoadModel::Params params) {
            model = std::make_unique<dummy::Model>(ModelParams_fromSchema(params));
            return {};
        }
    };

    try {
        auto io = co_await coro::Io{};

        co_await io.pushFrame(Frame_stateChange(Schema::id));

        Runner runner;

        while (true) {
            auto f = co_await io.pollFrame();
            co_await io.pushFrame(runner.dispatch(f.frame));
            if (runner.model) {
                co_await Dummy_runModel(io, astl::move(runner.model));
            }
        }
    }
    catch (coro::IoClosed&) {
        co_return;
    }
}

class DummyProvider final : public Provider {
public:
    virtual const Info& info() const noexcept override {
        static Info i = {
            .name = "ac-local dummy",
            .vendor = "Alpaca Core",
        };
        return i;
    }

    virtual SessionHandlerPtr createSessionHandler(std::string_view) override {
        return CoroSessionHandler::create(Dummy_runSession());
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
