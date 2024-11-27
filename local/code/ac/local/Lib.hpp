// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Version.hpp"
#include "ModelPtr.hpp"
#include "ProgressCb.hpp"
#include "LoadPluginCb.hpp"
#include <ac/DictFwd.hpp>
#include <string>
#include <string_view>

namespace ac::local {
class ModelLoaderRegistry;
class ModelLoader;
class ModelLoaderScorer;
struct ModelAssetDesc;

class PluginManager;
struct PluginInfo;

struct AC_LOCAL_EXPORT Lib {
    static ModelLoaderRegistry& modelLoaderRegistry();

    static void addLoader(ModelLoader& loader);

    static ModelPtr loadModel(ModelAssetDesc desc, Dict params, ProgressCb cb = {});
    static ModelPtr loadModel(const ModelLoaderScorer& scorer, ModelAssetDesc desc, Dict params, ProgressCb cb = {});

    static PluginManager& pluginManager();

    static void addPluginDir(std::string_view dir);
    static void addPluginDirsFromEnvVar(std::string envVar);

    static const PluginInfo* loadPlugin(const std::string& path);
    static void loadPlugins(LoadPluginCb cb = {});
    static void loadAllPlugins() { return loadPlugins({}); }
};

} // namespace ac::local
