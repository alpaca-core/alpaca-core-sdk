// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Lib.hpp"
#include "ModelLoaderRegistry.hpp"
#include "PluginManager.hpp"

namespace ac::local {

namespace {
ModelLoaderRegistry g_modelLoaderRegistry("global");
PluginManager g_pluginManager(g_modelLoaderRegistry);
} // namespace

ModelLoaderRegistry& Lib::modelLoaderRegistry() {
    return g_modelLoaderRegistry;
}

PluginManager& Lib::pluginManager() {
    return g_pluginManager;
}

} // namespace ac::local
