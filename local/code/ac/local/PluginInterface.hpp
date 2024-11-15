// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "ModelLoaderPtr.hpp"
#include <astl/version.hpp>
#include <vector>

namespace ac::local {
class ModelLoaderRegistry;

struct PluginInterface {
    using PluginLoadFunc = PluginInterface(*)();

    astl::version acLocalVersion; // version of ac-local this plugin was built with

    astl::version pluginVersion; // version of the plugin itself

    using GetLoadersFunc = std::vector<ModelLoaderPtr>(*)();
    GetLoadersFunc getLoaders;
};

}
