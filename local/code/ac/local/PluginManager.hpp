// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "PluginInfo.hpp"
#include "LoadPluginCb.hpp"

#include <string_view>

namespace ac::local {
class ProviderRegistry;
struct PluginInterface;

class AC_LOCAL_EXPORT PluginManager {
public:
    PluginManager(ProviderRegistry& registry);
    ~PluginManager();

    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;

    static std::string_view pluginPathToName(std::string_view path);

    ProviderRegistry& providerRegistry() const noexcept { return m_registry; }
    const std::vector<PluginInfo>& plugins() const noexcept { return m_plugins; }
    const std::vector<std::string>& pluginDirs() const noexcept { return m_pluginDirs; }

    void addPluginDir(std::string_view dir);
    void addPluginDirsFromEnvVar(std::string envVar);

    // load from path (including filename), ignores pluginDirs
    const PluginInfo* loadPlugin(const std::string& path, LoadPluginCb cb = {});

    // load all plugins from registered directories
    void loadPlugins(LoadPluginCb cb = {});

private:
    const PluginInfo* tryLoadPlugin(const std::string& path, LoadPluginCb& cb);

    ProviderRegistry& m_registry;

    std::vector<std::string> m_pluginDirs;

    std::vector<PluginInfo> m_plugins;
};

} // namespace ac::local
