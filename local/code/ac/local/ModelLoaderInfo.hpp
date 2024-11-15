// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <string>
#include <vector>

#include "ModelLoader.hpp"

namespace ac::local {

struct PluginInfo;

struct ModelLoaderInfo {
    PluginInfo* plugin = nullptr; // plugin that provided this loader (or null if directly added)

    std::string name; // human readable name of the loader (not necessarily unique across loaders)
    std::string vendor; // optional human readable name of the loader vendor

    std::string schemaType; // schema type the loaded models are expected to conform to
    std::string assetType; // asset type for the models

    std::vector<std::string> tags; // tags that can be used to filter loaders

    std::unique_ptr<ModelLoader> loader; // never null
};

} // namespace ac::local
