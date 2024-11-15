// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Version.hpp"
#include "ModelLoaderInfo.hpp"
#include <vector>

namespace ac::local {
class ModelLoaderRegistry;

struct PluginInterface {
    using PluginLoadFunc = PluginInterface(*)();

    Version acLocalVersion; // version of ac-local this plugin was built with

    Version pluginVersion; // version of the plugin itself

    std::vector<ModelLoaderInfo> (*getLoaders)();
};

}
