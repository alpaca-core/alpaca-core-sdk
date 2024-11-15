// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

// inline file - no include guard
#include <ac/local/Lib.hpp>
#include <ac/local/ModelLoaderRegistry.hpp>
#include <ac/local/PluginInterface.hpp>

namespace {
using GLF = ac::local::PluginInterface::GetLoadersFunc;
std::vector<ac::local::ModelLoaderPtr> g_loaders;
bool g_addedToGlobalRegistry = false;

void fillLoaders(GLF getLoaders) {
    if (!g_loaders.empty()) {
        // already filled
        return;
    }
    g_loaders = getLoaders();
}

void addLoadersToRegistry(GLF getLoaders, ac::local::ModelLoaderRegistry& registry) {
    fillLoaders(getLoaders);
    for (auto& loader : g_loaders) {
        registry.addLoader(*loader);
    }
}

void addLoadersToGlobalRegistry(GLF getLoaders) {
    if (g_addedToGlobalRegistry) {
        // already added
        return;
    }
    addLoadersToRegistry(getLoaders, ac::local::Lib::modelLoaderRegistry());
    g_addedToGlobalRegistry = true;
}

const auto& getGLoaders(GLF getLoaders) {
    fillLoaders(getLoaders);
    return g_loaders;
}

} // namespace


