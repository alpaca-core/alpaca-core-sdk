// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Lib.hpp"
#include "ModelLoaderRegistry.hpp"
#include "PluginManager.hpp"
#include "ModelAssetDesc.hpp"
#include <ac/Dict.hpp>
#include <astl/move.hpp>

namespace ac::local {

namespace {
ModelLoaderRegistry g_modelLoaderRegistry("global");
PluginManager g_pluginManager(g_modelLoaderRegistry);
} // namespace

ModelLoaderRegistry& Lib::modelLoaderRegistry() {
    return g_modelLoaderRegistry;
}

void Lib::addLoader(ModelLoader& loader) {
    g_modelLoaderRegistry.addLoader(loader);
}

ModelPtr Lib::loadModel(ModelAssetDesc desc, Dict params, ProgressCb cb) {
    return g_modelLoaderRegistry.loadModel(astl::move(desc), astl::move(params), astl::move(cb));
}

ModelPtr Lib::loadModel(const ModelLoaderScorer& scorer, ModelAssetDesc desc, Dict params, ProgressCb cb) {
    return g_modelLoaderRegistry.loadModel(scorer, astl::move(desc), astl::move(params), astl::move(cb));
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
