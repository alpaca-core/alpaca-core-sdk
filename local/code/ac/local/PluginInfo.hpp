// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Version.hpp"
#include "ModelLoaderPtr.hpp"
#include <vector>

namespace ac::local {

struct PluginInfo {
    std::string fullPath;
    std::stringView name; // points within fullPath, filename without "aclp-" prefix and extension

    void* nativeHandle; // HGLOBAL on Windows, void* on *nix

    Version version;

    std::vector<ModelLoaderPtr> loaders; // provided loaders
};

} // namespace ac::local
