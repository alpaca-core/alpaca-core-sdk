// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/AssetSourceLocalDir.hpp>
#include "ac-repo-root.h"
#include <doctest/doctest.h>

TEST_CASE("AssetSourceLocalDir") {
    auto src = ac::AssetSourceLocalDir_Create(AC_REPO_ROOT);
    REQUIRE(src);

    const std::string root = AC_REPO_ROOT;

    CHECK(src->id() == "local-dir: " AC_REPO_ROOT);

    {
        auto info = src->checkAssetSync("no-such-file");
        CHECK_FALSE(info);
    }

    {
        auto info = src->fetchAssetSync("no-such-file", {});
        REQUIRE_FALSE(info);
        CHECK(info.error() == "Asset not found");
    }

    const std::string asset = "VSOpenFileFromDirFilters.json";

    auto checkSuccess = [&](auto& info) {
        REQUIRE(info);
        CHECK(info->path.value_or(std::string{}) == root + "/" + asset);
        CHECK(info->size.value_or(0) == 86);
    };

    {
        auto info = src->checkAssetSync(asset);
        checkSuccess(info);
    }

    {
        auto info = src->fetchAssetSync(asset, {});
        checkSuccess(info);
    }
}
