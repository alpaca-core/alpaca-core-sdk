// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/AssetSourceLocalDir.hpp>
#include "TestBinaryAssets.hpp"
#include <doctest/doctest.h>

TEST_CASE("AssetSourceLocalDir") {

    auto src = ac::AssetSourceLocalDir_Create(Test_Local_Asset_Source_Path);
    REQUIRE(src);

    const std::string root = Test_Local_Asset_Source_Path;

    CHECK(src->id() == "local-dir: " + Test_Local_Asset_Source_Path);

    {
        auto info = src->checkAssetSync("no-such-file");
        CHECK_FALSE(info);
    }

    {
        auto info = src->fetchAssetSync("no-such-file", {});
        REQUIRE_FALSE(info);
        CHECK(info.error() == "Asset not found");
    }

    const std::string asset = Test_Local_Asset_Id;

    auto checkSuccess = [&](auto& info) {
        REQUIRE(info);
        CHECK(info->path.value_or(std::string{}) == root + "/" + asset);
        CHECK(info->size.value_or(0) == Test_Local_Asset_Size); // The size of our new binary file
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
