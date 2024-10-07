// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <string>
#include <vector>

namespace ac::local {

/// Model description. Used by `ModelFactory` and `ModelLoader` to load models.
struct ModelDesc {
    /// Inference type. Used by `ModelFactory` to select the appropriate loader. May also be used by `ModelLoader`.
    std::string inferenceType;

    struct AssetInfo {
        std::string path; ///< Path to the asset
        std::string tag;  ///< Tag of the asset. Some model loaders may require this to be set.
    };
    std::vector<AssetInfo> assets; ///< List of assets required by the model

    /// Not used by the library in any way besides logs and may be helpful for debugging
    std::string name;
};

} // namespace ac::local
