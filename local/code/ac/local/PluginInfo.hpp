// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "ModelLoaderPtr.hpp"
#include <astl/version.hpp>
#include <vector>

namespace ac::local {

struct PluginInfo {
    std::string fullPath;
    std::string name; // part of fullPath, filename without "aclp-" prefix and extension

    // data obtained from loaded plugin

    void* nativeHandle; // HGLOBAL on Windows, void* on *nix

    std::string label;
    std::string desc;
    std::string vendor;
    astl::version version = {};

    std::vector<ModelLoaderPtr> loaders; // provided loaders

    bool addedToGlobalRegistry = false;
};

} // namespace ac::local
