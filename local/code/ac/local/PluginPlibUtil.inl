// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

// inline file - no include guard
#include <ac/local/Lib.hpp>
#include <ac/local/ModelLoaderRegistry.hpp>
#include <ac/local/PluginInterface.hpp>

namespace {
using namespace ac::local;

struct PlibHelper {
    PluginInterface m_pluginInterface;
    PlibHelper(const PluginInterface& pluginInterface) : m_pluginInterface(pluginInterface) {}

    std::vector<ModelLoaderPtr> m_loaders;
    bool m_addedToGlobalRegistry = false;

    void fillLoaders() {
        if (!m_loaders.empty()) {
            // already filled
            return;
        }
        if (m_pluginInterface.init) {
            m_pluginInterface.init();
        }
        m_loaders = m_pluginInterface.getLoaders();
    }

    void addLoadersToRegistry(ac::local::ModelLoaderRegistry& registry) {
        fillLoaders();
        for (auto& loader : m_loaders) {
            registry.addLoader(*loader);
        }
    }

    void addLoadersToGlobalRegistry() {
        if (m_addedToGlobalRegistry) {
            // already added
            return;
        }
        addLoadersToRegistry(ac::local::Lib::modelLoaderRegistry());
        m_addedToGlobalRegistry = true;
    }

    const auto& getLoaders() {
        fillLoaders();
        return m_loaders;
    }
};

} // namespace


