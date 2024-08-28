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
    Dict baseParams;
    std::vector<std::string> assets;
};

}
