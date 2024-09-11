// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "AssetSource.hpp"
#include <astl/tsumap.hpp>
#include <cstdint>
#include <concepts>

namespace ac {
class AC_LOCAL_EXPORT AssetSourceHttp final : public AssetSource {
public:
    struct ManifestEntry {
        std::string url;
        std::optional<uint64_t> xxhash; // expected xxhash of the asset (if available in the manifest)
    };

    AssetSourceHttp(std::string id, std::string edownloadDir);

    void addAsset(std::string id, ManifestEntry entry);

    virtual std::string_view id() const noexcept override { return m_id; }

    virtual std::optional<BasicAssetInfo> checkAssetSync(std::string_view id) noexcept override;
    virtual BasicAssetInfo fetchAssetSync(std::string_view id, ProgressCb pcb) override;
private:
    std::string m_id;

    std::string m_downloadDir;

    struct AssetManifestEntry : public ManifestEntry {
        std::string targetPath; // where the asset should be stored

        // materialized info to return from interface methods
        BasicAssetInfo info;
    };

    astl::tsumap<AssetManifestEntry> m_assetManifest;
};
} // namespace ac
