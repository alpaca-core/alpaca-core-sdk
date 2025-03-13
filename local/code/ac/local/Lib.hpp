// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Version.hpp"
#include "LoadPluginCb.hpp"
#include <ac/DictFwd.hpp>
#include <string>
#include <string_view>

namespace ac::local {
class ServiceFactory;

class PluginManager;
struct PluginInfo;

struct AC_LOCAL_EXPORT Lib {
    static std::vector<const ServiceFactory*>& freeServiceFactories();
    static void registerService(ServiceFactory& factory);

    static PluginManager& pluginManager();

    static void addPluginDir(std::string_view dir);
    static void addPluginDirsFromEnvVar(std::string envVar);

    static const PluginInfo* loadPlugin(const std::string& path);
    static void loadPlugins(LoadPluginCb cb = {});
    static void loadAllPlugins() { return loadPlugins({}); }
};

} // namespace ac::local
