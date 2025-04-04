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
#include <ac/local/BackendWorkerStrand.hpp>
#include <ac/local/ResourceCache.hpp>
#include <ac/local/AssetMgrCoroUtil.hpp>

#include <ac/local/schema/AssetMgrInterface.hpp>

#include <ac/schema/FrameHelpers.hpp>
#include <ac/schema/StateChange.hpp>
#include <ac/schema/Error.hpp>
#include <ac/schema/OpTraits.hpp>

#include <ac/frameio/IoEndpoint.hpp>

#include <ac/xec/coro.hpp>
#include <ac/xec/co_spawn.hpp>
#include <ac/io/exception.hpp>

#include <astl/move.hpp>
#include <astl/throw_stdex.hpp>
#include <astl/iile.h>

#include <deque>

namespace ac::local {

namespace {

namespace sc = schema::dummy;

using namespace ac::frameio;

struct DummyModelResource : public dummy::Model, public Resource {
    using dummy::Model::Model;

    using Cache = ResourceCache<dummy::Model::Params, DummyModelResource>;
    using Lock = ResourceLock<DummyModelResource>;
};

struct Dummy {
    Backend& m_backend;
    DummyModelResource::Cache& m_resourceCache;
public:
    Dummy(Backend& backend, DummyModelResource::Cache& rm)
        : m_backend(backend)
        , m_resourceCache(rm)
    {}

    static Frame unknownOpError(const Frame& f) {
        return Frame_from(schema::Error{}, "dummy: unknown op: " + f.op);
    }

    xec::coro<Frame> runStream(IoEndpoint& io, astl::generator<const std::string&>& session) {
        using Schema = sc::StateInstance::OpStream;

        auto ret = Frame_from(schema::OpReturn<Schema>{}, {});

        Frame abortFrame;
        for (auto& w : session) {
            if (schema::Abort_check(io)) break;
            co_await io.push(Frame_from(Schema::StreamToken{}, w));
        }

        co_return ret;
    }

    xec::coro<void> runInstance(IoEndpoint& io, dummy::Model& model, const sc::StateModelLoaded::OpCreateInstance::Params& ciParams) {
        auto iparams = iile([&] {
            dummy::Instance::InitParams ret;
            ret.cutoff = ciParams.cutoff;
            return ret;
        });
        auto instance = std::make_unique<dummy::Instance>(model, iparams);

        using Schema = sc::StateInstance;
        co_await io.push(Frame_from(schema::StateChange{}, Schema::id));

        auto createSession = [&instance](Schema::InferenceParams& params) {
            dummy::Instance::SessionParams sparams;
            sparams.splice = params.splice;
            sparams.throwOn = params.throwOn;

            return instance->newSession(std::move(params.input), sparams);
        };

        while (true) {
            auto f = co_await io.poll();
            Frame fret;

            try {
                if (auto run = Frame_optTo(schema::OpParams<Schema::OpRun>{}, *f)) {
                    auto s = createSession(*run);

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

                    fret = Frame_from(schema::OpReturn<Schema::OpRun>{}, ret);
                }
                else if (auto stream = Frame_optTo(schema::OpParams<Schema::OpStream>{}, * f)) {
                    auto s = createSession(*stream);
                    fret = co_await runStream(io, s);
                }
                else {
                    fret = unknownOpError(*f);
                }
            }
            catch (std::runtime_error& e) {
                fret = Frame_from(schema::Error{}, e.what());
            }

            co_await io.push(fret);
        }
    }

    xec::coro<void> runModel(IoEndpoint& io, sc::StateDummy::OpLoadModel::Params& lmParams) {
        dummy::Model::Params mparams;

        if (lmParams.assets.hasValue() && !lmParams.assets->empty()) {
            auto assets = co_await AssetMgr_makeAssetsAvailable(m_backend, io, std::move(lmParams.assets.value()));
            if (!assets.empty()) {
                mparams.path = std::move(assets.front().uri.value());
            }
        }

        mparams.splice = astl::move(lmParams.spliceString.valueOr(""));

        io.put(Frame_from(schema::Progress{}, {.progress = 0.0f, .tag = mparams.path, .action = "loading"}));

        auto model = m_resourceCache.find(mparams);

        if (!model) {
            io.put(Frame_from(schema::Progress{}, {.progress = 0.1f, .tag = mparams.path, .action = "loading"}));
            model = m_resourceCache.add(
                mparams,
                std::make_shared<DummyModelResource>(mparams)
            );
        }

        using Schema = sc::StateModelLoaded;
        co_await io.push(Frame_from(schema::StateChange{}, Schema::id));

        while (true) {
            auto f = co_await io.poll();
            Frame err;

            try {
                if (auto ciParams = Frame_optTo(schema::OpParams<Schema::OpCreateInstance>{}, *f)) {
                    co_await runInstance(io, *model, *ciParams);
                }
                else {
                    err = unknownOpError(*f);
                }
            }
            catch (std::runtime_error& e) {
                err = Frame_from(schema::Error{}, e.what());
            }

            co_await io.push(err);
        }
    }

    xec::coro<void> runSession(IoEndpoint& io) {
        using Schema = sc::StateDummy;

        co_await io.push(Frame_from(schema::StateChange{}, Schema::id));

        while (true) {
            auto f = co_await io.poll();

            Frame err;

            try {
                if (auto lm = Frame_optTo(schema::OpParams<Schema::OpLoadModel>{}, *f)) {
                    co_await runModel(io, *lm);
                }
                else {
                    err = unknownOpError(*f);
                }
            }
            catch (std::runtime_error& e) {
                err = Frame_from(schema::Error{}, e.what());
            }

            co_await io.push(err);
        }
    }

    xec::coro<void> run(std::shared_ptr<void> self, frameio::StreamEndpoint ep) {
        try {
            auto ex = co_await xec::executor{};
            IoEndpoint io(std::move(ep), ex);

            co_await runSession(io);
        }
        catch (io::stream_closed_error&) {
            co_return;
        }
    }
};

ServiceInfo g_serviceInfo = {
    .name = "ac-local dummy",
    .vendor = "Alpaca Core",
};

struct DummyService final : public Service {
    DummyService(BackendWorkerStrand& ws) : m_workerStrand(ws) {}

    BackendWorkerStrand& m_workerStrand;
    DummyModelResource::Cache m_resourceCache{m_workerStrand.resourceManager};

    virtual const ServiceInfo& info() const noexcept override {
        return g_serviceInfo;
    }

    virtual void createSession(frameio::StreamEndpoint ep, Dict) override {
        auto dummy = std::make_shared<Dummy>(m_workerStrand.backend, m_resourceCache);
        co_spawn(m_workerStrand.executor(), dummy->run(dummy, std::move(ep)));
    }
};

struct DummyServiceFactory final : public ServiceFactory {
    virtual const ServiceInfo& info() const noexcept override {
        return g_serviceInfo;
    }
    virtual std::unique_ptr<Service> createService(Backend& backend) const override {
        auto svc = std::make_unique<DummyService>(backend.cpuWorkerStrand());
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
