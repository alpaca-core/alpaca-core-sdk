// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "AssetMgrService.hpp"
#include "schema/AssetMgrInterface.hpp"

#include "Service.hpp"
#include "ServiceFactory.hpp"
#include "ServiceInfo.hpp"

#include "Backend.hpp"

#include "fs/FsUtil.hpp"
#include "fs/FileUri.hpp"

#include <ac/schema/FrameHelpers.hpp>
#include <ac/schema/StateChange.hpp>
#include <ac/schema/Error.hpp>
#include <ac/schema/OpTraits.hpp>

#include <ac/frameio/IoEndpoint.hpp>
#include <ac/xec/timer_ptr.hpp>
#include <ac/xec/coro.hpp>
#include <ac/xec/co_spawn.hpp>

#include <furi/furi.hpp>

namespace ac::local {

namespace {

ServiceInfo g_serviceInfo = {
    .name = schema::Interface::id,
    .vendor = "Alpaca Core",
};

class AssetManager {
public:
    ac::xec::timer_wobj wobj;

    AssetManager(const xec::strand& ex)
        : wobj(ex)
    {}

    xec::coro<void> runService(std::shared_ptr<void> self) {
        [[maybe_unused]] auto aborted = co_await wobj.wait();
        assert(aborted);
    }

    using Schema = schema::amgr::State;

    xec::coro<Frame> makeAssetsAvailable(frameio::IoEndpoint& io, Schema::OpMakeAssetsAvailable::Params assets) {
        for (auto& asset : assets) {
            schema::Progress::Type progress = {.progress = 0.f, .tag = asset.uri.value(), .action = "fetch"};
            io.put(Frame_from(schema::Progress{}, progress));

            auto scheme = furi::uri_split::get_scheme_from_uri(asset.uri.value());
            if (scheme == "file") {
                auto path = fs::FileUri_toPath(asset.uri.value());

                auto stat = fs::basicStat(path);
                if (!stat.file()) {
                    throw_ex{} << "asset-mgr: file not found: " << path;
                }

                asset.uri = path;
            }
            else {
                throw_ex{} << "asset-mgr: unsupported URI scheme: " << asset.uri.value();
            }

            progress.progress = 1.f;
            io.put(Frame_from(schema::Progress{}, progress));
        }
        co_return Frame_from(schema::OpReturn<Schema::OpMakeAssetsAvailable>{}, std::move(assets));
    }

    xec::coro<void> runSession(frameio::IoEndpoint io) {
        try {
            co_await io.push(Frame_from(schema::StateChange{}, Schema::id));

            while (true) {
                auto f = co_await io.poll();

                Frame ret;

                try {
                    if (auto aa = Frame_optTo(schema::OpParams<Schema::OpMakeAssetsAvailable>{}, *f)) {
                        ret = co_await makeAssetsAvailable(io, *aa);
                    }
                    else {
                        ret = Frame_from(schema::Error{}, "asset-mgr: unknown op: " + f->op);
                    }
                }
                catch (std::runtime_error& e) {
                    ret = Frame_from(schema::Error{}, e.what());
                }

                co_await io.push(ret);
            }
        }
        catch (io::stream_closed_error&) {
            co_return;
        }
    }

    void launchSession(frameio::StreamEndpoint ep) {
        const auto& strand = wobj.get_executor();
        frameio::IoEndpoint xep(std::move(ep), strand);
        co_spawn(strand, runSession(std::move(xep)));
    }
};

struct AssetMgrService final : public Service {
    std::shared_ptr<AssetManager> m_assetManager;

    AssetMgrService(const xec::strand& s)
        : m_assetManager(std::make_shared<AssetManager>(s))
    {
        co_spawn(s, m_assetManager->runService(m_assetManager));
    }

    ~AssetMgrService() override {
        m_assetManager->wobj.notify_all();
    }

    virtual const ServiceInfo& info() const noexcept override {
        return g_serviceInfo;
    }
    virtual void createSession(frameio::StreamEndpoint ep, Dict) override {
        m_assetManager->launchSession(std::move(ep));
    }
};

class AssetMgrServiceFactory final : public ServiceFactory {
    virtual const ServiceInfo& info() const noexcept override {
        return g_serviceInfo;
    }
    virtual std::unique_ptr<Service> createService(Backend& backend) const override {
        auto svc = std::make_unique<AssetMgrService>(backend.xctx().io.make_strand());
        return svc;
    }
};
} // namespace

void registerAssetMgrService(Backend& be) {
    static AssetMgrServiceFactory factory;
    be.registerService(factory);
}

} // namespace ac::local
