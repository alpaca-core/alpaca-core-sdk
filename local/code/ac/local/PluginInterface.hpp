// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Version.hpp"

namespace ac::local {
class ModelFactory;

struct PluginInterface {
    using PluginLoadFunc = PluginInterface(*)();

    Version acLocalVersion;

    Version pluginVersion;

    void (*addLoadersToFactory)(ModelFactory& factory);
};

}
