// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Lib.hpp"
#include "PluginManager.hpp"
#include <astl/move.hpp>

namespace ac::local {

namespace {
std::vector<const ServiceFactory*> g_factories;
PluginManager g_pluginManager("global");
} // namespace

std::vector<const ServiceFactory*>& Lib::freeServiceFactories() {
    return g_factories;
}

void Lib::registerService(ServiceFactory& factory) {
    g_factories.push_back(&factory);
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

} // namespace ac::local
