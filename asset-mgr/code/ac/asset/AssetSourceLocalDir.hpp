// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "AssetSource.hpp"
#include <string>
#include <memory>

namespace ac::asset {
AC_ASSET_EXPORT std::unique_ptr<AssetSource> AssetSourceLocalDir_Create(std::string path);
}
