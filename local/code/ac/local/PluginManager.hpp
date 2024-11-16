// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "PluginInfo.hpp"
#include <astl/tsumap.hpp>

namespace ac::local {

class AC_LOCAL_EXPORT PluginManager {
public:
    PluginManager();
    ~PluginManager();

    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;

private:
    // map full path to plugin info
    astl::tsumap<PluginInfo> m_plugins;
};

} // namespace ac::local
