// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Version.hpp"
#include "LoadPluginCb.hpp"
#include <ac/DictFwd.hpp>
#include <ac/frameio/SessionHandlerPtr.hpp>
#include <string>
#include <string_view>

namespace ac::local {
class ProviderRegistry;
class Provider;
class ProviderScorer;

class PluginManager;
struct PluginInfo;

struct AC_LOCAL_EXPORT Lib {
    static ProviderRegistry& providerRegistry();

    static void addProvider(Provider& provider);

    static PluginManager& pluginManager();

    static void addPluginDir(std::string_view dir);
    static void addPluginDirsFromEnvVar(std::string envVar);

    static const PluginInfo* loadPlugin(const std::string& path);
    static void loadPlugins(LoadPluginCb cb = {});
    static void loadAllPlugins() { return loadPlugins({}); }

    static frameio::SessionHandlerPtr createSessionHandler(std::string_view matchName);

    Provider& getProvider(std::string_view nameMatch);
};

} // namespace ac::local
