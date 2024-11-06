// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

namespace ac::local {
class ModelFactory;

struct PluginInterface {
    using PluginLoadFunc = PluginInterface(*)();

    void (*addLoadersToFactory)(ModelFactory& factory);
};

}
