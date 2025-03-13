// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <astl/version.hpp>
#include <vector>
#include <string>

namespace ac::local {

class ServiceFactory;

struct PluginInfo {
    std::string fullPath; // full path to the plugin file, only unique member
    std::string name; // part of fullPath, filename without "aclp-" prefix and extension

    // data obtained from loaded plugin

    void* nativeHandle; // HGLOBAL on Windows, void* on *nix

    std::string label;
    std::string desc;
    std::string vendor;
    astl::version version = {};

    std::vector<std::string> tags;

    // plugin-specific raw data
    void* rawData = nullptr;

    std::vector<const ServiceFactory*> serviceFactories; // provided service factories
};

} // namespace ac::local
