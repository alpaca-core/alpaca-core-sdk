// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "AssetSource.hpp"
#include <astl/tsumap.hpp>
#include <cstdint>

namespace ac {
class AC_LOCAL_EXPORT AssetSourceHttp final : public AssetSource {
public:
    AssetSourceHttp();

    virtual std::optional<BasicAssetInfo> checkAssetSync(std::string_view id) noexcept override;
    virtual BasicAssetInfo fetchAssetSync(std::string_view id, ProgressCb) override;
private:
    struct AssetManifestEntry {
        std::string url;
        uint64_t xxhash; // xxhash of the asset
        std::string targetPath; // where the asset should be stored

        // materialized info to return from interface methods
        BasicAssetInfo info;
    };

    astl::tsumap<AssetManifestEntry> m_assetManifest;
};
} // namespace ac
