// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "AssetSourceHttp.hpp"
#include <stdexcept>

namespace ac {

AssetSourceHttp::AssetSourceHttp() {}

std::optional<AssetSource::BasicAssetInfo> AssetSourceHttp::checkAssetSync(std::string_view id) noexcept {
    return {};
}

AssetSource::BasicAssetInfo AssetSourceHttp::fetchAssetSync(std::string_view id, ProgressCb pcb) {
    throw std::runtime_error("Not implemented");
}

} // namespace ac
