// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"

#include <ac/frameio/StreamEndpointFwd.hpp>
#include <ac/xec/context.hpp>

#include <memory>
#include <vector>
#include <span>
#include <string>
#include <string_view>

namespace ac::frameio {
struct ChannelEndpoints;
}

namespace ac::local {

class Service;
class ServiceFactory;
struct PluginInfo;
class PluginManager;

// ideally this would be a nested type in Backend, but then a clang bug is triggered:
// https://bugs.llvm.org/show_bug.cgi?id=36684
// to work around this, we have the type external
struct BackendIoBufferSizes {
    size_t localToRemote = 10;
    size_t remoteToLocal = 10;
};

class AC_LOCAL_EXPORT Backend {
public:
    Backend(const Backend&) = delete;
    Backend& operator=(const Backend&) = delete;

    struct ServiceData {
        ServiceData(const ServiceFactory& factory, const PluginInfo* pluginInfo)
            : service(nullptr)
            , factory(factory)
            , pluginInfo(pluginInfo)
        {}

        Service* service; // may be null if service is not instantiated
        const ServiceFactory& factory;
        const PluginInfo* pluginInfo; // may be null for directly added services
    };

    void registerService(const ServiceFactory& factory, const PluginInfo* pluginInfo = nullptr);

    void registerServices(std::span<const ServiceFactory* const> factories, const PluginInfo* pluginInfo = nullptr) {
        for (const auto* factory : factories) {
            registerService(*factory, pluginInfo);
        }
    }

    void registerPluginServices(const PluginInfo& pluginInfo);
    void registerPluginServices(const PluginManager& pluginManager);

    void registerLibServices();

    struct Xctx {
        xec::context& system;
        xec::context& io;
        xec::context& dispatch;
        xec::strand cpu;
        xec::strand gpu;
    };

    const Xctx& xctx() const { return m_xctx; }

    static frameio::ChannelEndpoints getEndpoints(BackendIoBufferSizes bufferSizes = {});

    frameio::StreamEndpoint connect(std::string_view serviceNameMatch, BackendIoBufferSizes bufferSizes = {});
    void attach(std::string_view serviceNameMatch, frameio::StreamEndpoint ep);

protected:
    explicit Backend(std::string_view name, Xctx xctx);
    ~Backend();

private:
    std::string m_name;
    Xctx m_xctx;

    Service* getService(std::string_view serviceNameMatch);
    frameio::StreamEndpoint connect(Service& service, BackendIoBufferSizes bufferSizes);
    void attach(Service& service, frameio::StreamEndpoint ep);

    std::vector<ServiceData> m_serviceDatas;
    std::vector<std::unique_ptr<Service>> m_instantiatedServices;
};

} // namespace ac::local
