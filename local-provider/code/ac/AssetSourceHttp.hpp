// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "AssetSource.hpp"
#include <unordered_map>

namespace ac {
class AC_LOCAL_EXPORT AssetSourceHttp final : public AssetSource {
public:
    AssetSourceHttp();

    virtual std::optional<BasicAssetInfo> checkAssetSync(std::string_view id) override;
    virtual itlib::expected<BasicAssetInfo, std::string> fetchAssetSync(std::string_view id, ProgressCb) override;
private:
    struct AssetManifestEntry {
        std::string url;
        BasicAssetInfo info;
    };

    std::unordered_map<std::string, AssetManifestEntry> m_assetManifest;
};
} // namespace ac
