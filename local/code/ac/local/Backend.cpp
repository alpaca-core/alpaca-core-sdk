// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Backend.hpp"

#include "Service.hpp"
#include "ServiceFactory.hpp"
#include "ServiceInfo.hpp"
#include "PluginManager.hpp"
#include "Lib.hpp"
#include "Logging.hpp"
#include "BackendWorkerStrand.hpp"

#include "AssetMgrService.hpp"

#include <ac/frameio/local/BufferedChannelStream.hpp>
#include <ac/frameio/StreamEndpoint.hpp>
#include <ac/xec/coro.hpp>
#include <ac/xec/co_spawn.hpp>

#include <astl/throw_stdex.hpp>
#include <astl/id_ptr.hpp>

namespace ac::local {

namespace {
struct BackendWorkerStrandState : public BackendWorkerStrand {
    BackendWorkerStrandState(Backend& backend, std::string name, const xec::strand& ex)
        : BackendWorkerStrand(backend, std::move(name), ex)
    {}

    xec::coro<void> gc(std::shared_ptr<void> self) {
        while (true) {
            auto aborted = co_await m_wobj.wait(astl::timeout(std::chrono::seconds(10)));
            if (aborted) co_return;
            resourceManager.garbageCollect();
        }
    }
};
}

Backend::Backend(std::string_view name, Xctx xctx)
    : m_name(astl::id_or_ptr(name, this))
    , m_xctx(xctx)
{
    // temporary here until we have a better way to register services
    {
        registerAssetMgrService(*this);
        registerLibServices();
    }

    auto cpu = std::make_shared<BackendWorkerStrandState>(*this, "cpu", m_xctx.cpu);
    m_cpuWorkerStrand = cpu;
    co_spawn(m_xctx.cpu, cpu->gc(cpu));

    auto gpu = std::make_shared<BackendWorkerStrandState>(*this, "gpu", m_xctx.gpu);
    m_gpuWorkerStrand = gpu;
    co_spawn(m_xctx.gpu, gpu->gc(gpu));
}

Backend::~Backend() {
    m_cpuWorkerStrand->m_wobj.notify_one();
    m_gpuWorkerStrand->m_wobj.notify_one();
}

void Backend::registerService(const ServiceFactory& factory, const PluginInfo* pluginInfo) {
    std::lock_guard l(m_mutex);
    m_serviceDatas.emplace_back(factory, pluginInfo);
}

void Backend::registerPluginServices(const PluginInfo& pluginInfo) {
    registerServices(pluginInfo.serviceFactories, &pluginInfo);
}

void Backend::registerPluginServices(const PluginManager& pluginManager) {
    for (auto& p : pluginManager.plugins()) {
        registerPluginServices(p);
    }
}

void Backend::registerLibServices() {
    registerServices(Lib::freeServiceFactories());
    registerPluginServices(Lib::pluginManager());
}

frameio::ChannelEndpoints Backend::getEndpoints(BackendIoBufferSizes bufferSizes) {
    return frameio::BufferedChannel_getEndpoints(
        bufferSizes.localToRemote,
        bufferSizes.remoteToLocal
    );
}

frameio::StreamEndpoint Backend::connect(std::string_view serviceNameMatch, Dict target, BackendIoBufferSizes bufferSizes) {
    auto svc = getService(serviceNameMatch);
    if (!svc) {
        throw_ex{} << "Service not found: " << serviceNameMatch;
    }
    return connect(*svc, std::move(target), bufferSizes);
}

void Backend::attach(std::string_view serviceNameMatch, Dict target, frameio::StreamEndpoint ep) {
    auto svc = getService(serviceNameMatch);
    if (!svc) {
        throw_ex{} << "Service not found: " << serviceNameMatch;
    }
    attach(*svc, std::move(target), std::move(ep));
}

inline jalog::BasicStream& operator,(jalog::BasicStream& s, const std::vector<std::string>& vec) {
    s, "[";
    for (const auto& v : vec) {
        s, v, ", ";
    }
    s, "]";
    return s;
}

Service* Backend::getService(std::string_view serviceNameMatch) {
    std::lock_guard l(m_mutex);
    for (auto& sd : m_serviceDatas) {
        if (sd.factory.info().name.find(serviceNameMatch) != std::string::npos) {
            if (!sd.service) {
                auto& svc = m_instantiatedServices.emplace_back(sd.factory.createService(*this));

                auto& info = svc->info();
                AC_LOCAL_LOG(Info, "Backend ", m_name, " created service ", info.name,
                    "\n       vendor: ", info.vendor,
                    "\n         tags: ", info.tags
                );

                sd.service = svc.get();
            }
            return sd.service;
        }
    }
    return nullptr;
}

frameio::StreamEndpoint Backend::connect(Service& service, Dict target, BackendIoBufferSizes bufferSizes) {
    auto [local, remote] = getEndpoints(bufferSizes);
    attach(service, std::move(target), std::move(remote));
    return std::move(local);
}

void Backend::attach(Service& service, Dict target, frameio::StreamEndpoint ep) {
    service.createSession(std::move(ep), std::move(target));
}

} // namespace ac::local
