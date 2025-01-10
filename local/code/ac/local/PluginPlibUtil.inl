// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

// inline file - no include guard
#include <ac/local/Lib.hpp>
#include <ac/local/ProviderRegistry.hpp>
#include <ac/local/PluginInterface.hpp>

namespace {
using namespace ac::local;

struct PlibHelper {
    PluginInterface m_pluginInterface;
    PlibHelper(const PluginInterface& pluginInterface) : m_pluginInterface(pluginInterface) {}

    std::vector<ProviderPtr> m_loaders;
    bool m_addedToGlobalRegistry = false;

    void fillProviders() {
        if (!m_loaders.empty()) {
            // already filled
            return;
        }
        if (m_pluginInterface.init) {
            m_pluginInterface.init();
        }
        m_loaders = m_pluginInterface.getProviders();
    }

    void addProvidersToRegistry(ac::local::ProviderRegistry& registry) {
        fillProviders();
        for (auto& loader : m_loaders) {
            registry.addProvider(*loader);
        }
    }

    void addProvidersToGlobalRegistry() {
        if (m_addedToGlobalRegistry) {
            // already added
            return;
        }
        addProvidersToRegistry(ac::local::Lib::providerRegistry());
        m_addedToGlobalRegistry = true;
    }

    const auto& getProviders() {
        fillProviders();
        return m_loaders;
    }
};

} // namespace


