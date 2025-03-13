// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "PluginInfo.hpp"
#include "LoadPluginCb.hpp"

#include <string_view>

namespace ac::local {
struct PluginInterface;

class AC_LOCAL_EXPORT PluginManager {
public:
    PluginManager(std::string_view name = {});
    ~PluginManager();

    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;

    static std::string_view pluginPathToName(std::string_view path);

    const std::vector<PluginInfo>& plugins() const noexcept { return m_plugins; }
    const std::vector<std::string>& pluginDirs() const noexcept { return m_pluginDirs; }

    void addPluginDir(std::string_view dir);
    void addPluginDirsFromEnvVar(std::string envVar);

    // load from path (including filename), ignores pluginDirs
    const PluginInfo* loadPlugin(const std::string& path, LoadPluginCb cb = {});

    // load all plugins from registered directories
    void loadPlugins(LoadPluginCb cb = {});

    const PluginInfo* loadPlib(const PluginInterface& interface);

private:
    const PluginInfo* tryLoadPlugin(const std::string& path, LoadPluginCb& cb);

    const PluginInfo* tryCreatePluginInfo(
        const std::string& path,
        std::string_view name,
        void* nativeHandle,
        const PluginInterface& interface,
        LoadPluginCb& cb);

    std::string m_name;

    std::vector<std::string> m_pluginDirs;

    std::vector<PluginInfo> m_plugins;
};

} // namespace ac::local
