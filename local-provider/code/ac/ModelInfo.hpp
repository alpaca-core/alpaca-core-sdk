// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/Dict.hpp>
#include <string>
#include <vector>

namespace ac {

struct ModelInfo {
    std::string id;
    std::string inferenceType;

    struct Asset {
        std::string id;
        std::string tag;
    };
    std::vector<Asset> assets;
};

}
