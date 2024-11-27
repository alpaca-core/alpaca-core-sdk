// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <astl/safe_func.hpp>
#include <astl/ufunction.hpp>
#include <string_view>

namespace ac::local {
struct PluginInterface;
struct PluginInfo;

struct LoadPluginCb {
    template <typename Sig>
    using Func = astl::ufunction<Sig>;

    using PluginPathFilter = Func<bool(std::string_view path)>;
    PluginPathFilter pathFilter;
    using PluginNameFilter = Func<bool(std::string_view name)>;
    PluginNameFilter nameFilter;
    using PluginInterfaceFilter = Func<bool(const PluginInterface&)>;
    PluginInterfaceFilter interfaceFilter;
    using OnPluginLoaded = astl::safe_func<Func<void(const PluginInfo&)>>;
    OnPluginLoaded onPluginLoaded;
};
}