// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Version.hpp"

namespace ac::local {
class ModelFactory;

struct PluginInterface {
    using PluginLoadFunc = PluginInterface(*)();

    Version acLocalVersion; // version of ac-local this plugin was built with

    Version pluginVersion; // version of the plugin itself

    void (*addLoadersToFactory)(ModelFactory& factory);
};

}
