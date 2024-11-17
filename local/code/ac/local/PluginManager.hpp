// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "PluginInfo.hpp"
#include <string_view>
#include <functional>

namespace ac::local {
class ModelLoaderRegistry;
struct PluginInterface;

class AC_LOCAL_EXPORT PluginManager {
public:
    PluginManager(ModelLoaderRegistry& registry);
    ~PluginManager();

    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;

    ModelLoaderRegistry& modelLoaderRegistry() const noexcept { return m_registry; }
    const std::vector<PluginInfo>& plugins() const noexcept { return m_plugins; }
    const std::vector<std::string>& pluginDirs() const noexcept { return m_pluginDirs; }

    void addPluginDir(std::string_view dir);
    void addPluginDirsFromEnvVar(std::string envVar);

    struct LoadPluginCb {
        using PluginFileFilter = std::function<bool(std::string_view name)>;
        PluginFileFilter fileFilter = {};
        using PluginInterfaceFilter = std::function<bool(const PluginInterface&)>;
        PluginInterfaceFilter interfaceFilter = {};
        using OnPluginLoaded = std::function<void(const PluginInfo&)>;
        OnPluginLoaded onPluginLoaded = {};
    };

    // load from path (including filename)
    PluginInfo* loadPlugin(const std::string& path, LoadPluginCb cb = {});

    // load all plugins from registered directories
    void loadAllPlugins(LoadPluginCb cb = {});

    // load plugins whose name (without aclp-) starts with prefix
    void loadPlugins(std::string_view prefix, LoadPluginCb cb = {});

private:
    ModelLoaderRegistry& m_registry;

    std::vector<std::string> m_pluginDirs;

    std::vector<PluginInfo> m_plugins;
};

} // namespace ac::local
