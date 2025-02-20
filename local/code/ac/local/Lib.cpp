// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Lib.hpp"
#include "ProviderRegistry.hpp"
#include "PluginManager.hpp"
#include <ac/Dict.hpp>
#include <astl/move.hpp>

namespace ac::local {

namespace {
ProviderRegistry g_providerRegistry("global");
PluginManager g_pluginManager(g_providerRegistry);
} // namespace

ProviderRegistry& Lib::providerRegistry() {
    return g_providerRegistry;
}

void Lib::addProvider(Provider& provider) {
    g_providerRegistry.addProvider(provider);
}

PluginManager& Lib::pluginManager() {
    return g_pluginManager;
}

void Lib::addPluginDir(std::string_view dir) {
    g_pluginManager.addPluginDir(dir);
}

void Lib::addPluginDirsFromEnvVar(std::string envVar) {
    g_pluginManager.addPluginDirsFromEnvVar(envVar);
}

const PluginInfo* Lib::loadPlugin(const std::string& path) {
    return g_pluginManager.loadPlugin(path);
}

void Lib::loadPlugins(LoadPluginCb cb) {
    g_pluginManager.loadPlugins(astl::move(cb));
}

frameio::SessionHandlerPtr Lib::createSessionHandler(std::string_view matchName) {
    return g_providerRegistry.createSessionHandler(matchName);
}

Provider& getProvider(std::string_view nameMatch) {
    return g_providerRegistry.getProvider(nameMatch);
}

} // namespace ac::local
