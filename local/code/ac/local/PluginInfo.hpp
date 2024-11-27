// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "ModelLoaderPtr.hpp"
#include <astl/version.hpp>
#include <vector>

namespace ac::local {

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

    std::vector<ModelLoaderPtr> loaders; // provided loaders
};

} // namespace ac::local
