// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <string>
#include <vector>

namespace ac::local {

/// Model asset description. Used by `ModelLoader` to load models.
/// @ingroup cpp-local
struct ModelAssetDesc {
    /// Asset (weights) type. Used by loaders to check whether they can load the model.
    std::string type;

    /// Tags. May be used by loaders to guess whether they can load the model.
    std::vector<std::string> tags;

    /// Individual asset information.
    /// @ingroup cpp-local
    struct AssetInfo {
        std::string path; ///< Path to asset file
        std::string tag;  ///< Tag of the asset. Some loaders may require this to be set.
    };
    std::vector<AssetInfo> assets; ///< List of assets

    /// Model name tag.
    /// Not used by the library in any way besides logs and may be helpful for debugging
    std::string name;
};

} // namespace ac::local
