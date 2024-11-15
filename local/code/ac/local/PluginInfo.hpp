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
    std::stringView name; // points within fullPath, filename without "aclp-" prefix and extension

    // data obtained from loaded plugin

    void* nativeHandle; // HGLOBAL on Windows, void* on *nix

    std::string label;
    std::string desc;
    std::string vendor;
    version version;

    std::vector<ModelLoaderPtr> loaders; // provided loaders
};

} // namespace ac::local
