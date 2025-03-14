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

#include <ac/frameio/local/BufferedChannelStream.hpp>
#include <ac/frameio/StreamEndpoint.hpp>

#include <astl/throw_stdex.hpp>
#include <charconv>

namespace ac::local {

Backend::Backend(std::string_view name, Xctx xctx)
    : m_name(name)
    , m_xctx(xctx)
{
    if (m_name.empty()) {
        char hex[20] = "0x";
        auto r = std::to_chars(hex + 2, hex + sizeof(hex), reinterpret_cast<uintptr_t>(this), 16);
        m_name = std::string_view(hex, r.ptr - hex);
    }

    // temporary here until we have a better way to register services
    registerLibServices();
}

Backend::~Backend() = default;

void Backend::registerService(const ServiceFactory& factory, const PluginInfo* pluginInfo) {
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

frameio::StreamEndpoint Backend::connect(std::string_view serviceNameMatch, BackendIoBufferSizes bufferSizes) {
    auto svc = getService(serviceNameMatch);
    if (!svc) {
        throw_ex{} << "Service not found: " << serviceNameMatch;
    }
    return connect(*svc, bufferSizes);
}

void Backend::attach(std::string_view serviceNameMatch, frameio::StreamEndpoint ep) {
    auto svc = getService(serviceNameMatch);
    if (!svc) {
        throw_ex{} << "Service not found: " << serviceNameMatch;
    }
    attach(*svc, std::move(ep));
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

frameio::StreamEndpoint Backend::connect(Service& service, BackendIoBufferSizes bufferSizes) {
    auto [local, remote] = getEndpoints(bufferSizes);
    attach(service, std::move(remote));
    return std::move(local);
}

void Backend::attach(Service& service, frameio::StreamEndpoint ep) {
    service.createSession(std::move(ep));
}

} // namespace ac::local
