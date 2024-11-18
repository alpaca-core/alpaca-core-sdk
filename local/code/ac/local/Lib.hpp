// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Version.hpp"
#include <string>

namespace ac::local {
class ModelLoaderRegistry;
class ModelLoader;

class PluginManager;
struct PluginInfo;

struct AC_LOCAL_EXPORT Lib {
    static ModelLoaderRegistry& modelLoaderRegistry();

    static void addLoader(ModelLoader& loader);

    static PluginManager& pluginManager();

    static const PluginInfo* loadPlugin(const std::string& path);
    static void loadAllPlugins();
};

} // namespace ac::local
