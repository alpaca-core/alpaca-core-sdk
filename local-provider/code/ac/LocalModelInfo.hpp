// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "ModelInfo.hpp"
#include "AssetInfo.hpp"

namespace ac {
struct LocalModelInfo : public ModelInfo {
    std::vector<AssetInfo> localAssets;
};
} // namespace ac
