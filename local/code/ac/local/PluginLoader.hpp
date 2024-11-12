// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "PluginInterface.hpp"
#include <string_view>
#include <string>

namespace ac::local {
class AC_LOCAL_EXPORT PluginLoader {
public:
    static PluginInterface loadPlugin(std::string_view dir, std::string_view lib);
    static PluginInterface loadPlugin(const std::string& filename);
};
}
