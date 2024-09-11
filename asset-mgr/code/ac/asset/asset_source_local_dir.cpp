// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "asset_source_local_dir.h"
#include "AssetSourceLocalDir.hpp"

ac_asset_source* ac_new_asset_source_local_dir(const char* path) {
    return reinterpret_cast<ac_asset_source*>(ac::asset::AssetSourceLocalDir_Create(path).release());
}
