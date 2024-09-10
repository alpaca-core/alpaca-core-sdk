// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "AssetSource.hpp"
#include <astl/tsumap.hpp>

namespace ac {
class AC_LOCAL_EXPORT AssetSourceLocalFiles final : public AssetSource {
public:
    explicit AssetSourceLocalFiles(astl::tsumap<std::string> manifest);
    explicit AssetSourceLocalFiles(std::string jsonManifest);

    virtual std::optional<BasicAssetInfo> checkAssetSync(std::string_view id) noexcept override;
    virtual BasicAssetInfo fetchAssetSync(std::string_view id, ProgressCb) override;
private:
    astl::tsumap<std::string> m_assetManifest; // id -> path
    astl::tsumap<BasicAssetInfo> m_availableAssets; // id -> existing file
};
} // namespace ac
