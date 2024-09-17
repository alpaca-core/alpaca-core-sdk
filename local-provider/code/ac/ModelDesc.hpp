// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <string>
#include <vector>

namespace ac {

struct ModelDesc {
    std::string inferenceType;
    struct AssetInfo {
        std::string path;
        std::string tag;
    };
    std::vector<AssetInfo> assets;

    // this field is not used by the library in any way besides logs and may be helpful for debugging
    std::string name;
};

}
