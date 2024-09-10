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
        CHECK_THROWS_WITH_AS(src->fetchAssetSync("no-such-file", {}), "Asset not found", std::runtime_error);
    }

    const std::string asset = Test_Local_Asset_Id;

    auto checkSuccess = [&](ac::AssetSource::BasicAssetInfo& info) {
        CHECK(info.path.value_or(std::string{}) == root + "/" + asset);
        CHECK(info.size.value_or(0) == Test_Local_Asset_Size); // The size of our new binary file
    };

    {
        auto info = src->checkAssetSync(asset);
        REQUIRE(info);
        checkSuccess(*info);
    }

    {
        auto info = src->fetchAssetSync(asset, {});
        checkSuccess(info);
    }
}
