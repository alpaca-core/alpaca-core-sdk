// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "ModelInfo.hpp"
#include <ac/asset/Info.hpp>

namespace ac {
struct LocalModelInfo : public ModelInfo {
    std::vector<asset::AssetInfo> localAssets;
};
} // namespace ac
