// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "AssetSourceHttp.hpp"

namespace ac {

AssetSourceHttp::AssetSourceHttp() {}

std::optional<AssetSource::BasicAssetInfo> AssetSourceHttp::checkAssetSync(std::string_view id) {
    return {};
}

itlib::expected<AssetSource::BasicAssetInfo, std::string> AssetSourceHttp::fetchAssetSync(std::string_view id, ProgressCb pcb) {
    return itlib::unexpected("Not implemented");
}

} // namespace ac
