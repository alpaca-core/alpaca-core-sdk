// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "ModelLoaderPtr.hpp"
#include <astl/version.hpp>
#include <vector>

namespace ac::local {

struct PluginInterface {
    using GetAcLocalVersionFunc = int(*)();
    using GetFunc = PluginInterface(*)();

    const char* label; // optional own name (not necessarily unique)
    const char* desc; // optional short description
    const char* vendor; // optional vendor name
    astl::version version; // version of the plugin

    using InitFunc = void*(*)();
    InitFunc init; // optional init function, returns plugin user data

    using GetLoadersFunc = std::vector<ModelLoaderPtr>(*)();
    GetLoadersFunc getLoaders; // function to get loaders
};

}
