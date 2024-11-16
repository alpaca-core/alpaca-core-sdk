// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Version.hpp"

namespace ac::local {
class ModelLoaderRegistry;
class PluginManager;

struct AC_LOCAL_EXPORT Lib {
    static ModelLoaderRegistry& modelLoaderRegistry();
    static PluginManager& pluginManager();
};

} // namespace ac::local
