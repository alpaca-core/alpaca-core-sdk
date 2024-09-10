// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/AssetSourceLocalDir.hpp>
#include <ac/AssetManager.hpp>
#include "TestBinaryAssets.hpp"
#include <doctest/doctest.h>
#include <latch>


const auto Dummy_Another_Test_File_Size = Another_Test_Asset_Id.length() + 1000; //Size reported by the Dummy asset source

class DummyAssetSource : public ac::AssetSource {
public:
    virtual std::string_view id() const override {
        return "dummy";
    }

    virtual std::optional<BasicAssetInfo> checkAssetSync(std::string_view id) noexcept override {
        if (id.starts_with("test") || id.starts_with("no")) {
            return std::nullopt;
        }
        if (id.starts_with("local")) {
            return BasicAssetInfo{1'000'000 + id.size(), "loc/" + std::string(id)};
        }
        if (id.starts_with("sizeless")) {
            return BasicAssetInfo{};
        }
        return BasicAssetInfo{1'000 + id.size()};
    }
    virtual BasicAssetInfo fetchAssetSync(std::string_view id, ProgressCb progress) override {
        auto basicInfo = checkAssetSync(id);
        progress(7); // dummy progress so we can check it
        if (!basicInfo) {
            throw std::runtime_error("dummy not found");
        }
        if (id.starts_with("error")) {
            throw std::runtime_error("dummy error");
        }
        if (!basicInfo->path) {
            basicInfo->path = "dl/" + std::string(id);
        }
        if (!basicInfo->size) {
            basicInfo->size = 10'000 + id.length();
        }
        return std::move(*basicInfo);
    }
};

TEST_CASE("dummy-dir") {
    ac::AssetManager mgr;

    mgr.addSource(ac::AssetSourceLocalDir_Create(Test_Local_Asset_Source_Path), 10);
    mgr.addSource(std::make_unique<DummyAssetSource>());

    ac::AssetInfo info;
    auto q = [&](std::string_view qid) {
        std::latch latch(1);
        mgr.queryAsset(std::string(qid), [&](std::string_view id, const ac::AssetInfo& data) {
            CHECK(id == qid);
            info = data;
            latch.count_down();
        });
        latch.wait();
    };

    q("no-such-asset");
    CHECK_FALSE(info.source);
    CHECK_FALSE(info.size);
    CHECK_FALSE(info.path);
    CHECK(info.error == "Asset not found");

    q(Test_Local_Asset_Id);
    REQUIRE(info.source);
    CHECK(info.source->id() == "local-dir: " + Test_Local_Asset_Source_Path);
    CHECK(info.size == Test_Local_Asset_Size);
    CHECK(info.path == Test_Local_Asset_Source_Path + "/" + Test_Local_Asset_Id);
    CHECK_FALSE(info.error);

    q(Another_Test_Asset_Id);
    REQUIRE(info.source);
    CHECK(info.source->id() == "dummy"); // smaller prio
    CHECK(info.size == Dummy_Another_Test_File_Size);
    CHECK_FALSE(info.path);
    CHECK_FALSE(info.error);

    q("local-asset");
    REQUIRE(info.source);
    CHECK(info.source->id() == "dummy");
    CHECK(info.size == 1'000'000 + 11);
    CHECK(info.path == "loc/local-asset");
    CHECK_FALSE(info.error);

    q("sizeless-asset");
    REQUIRE(info.source);
    CHECK(info.source->id() == "dummy");
    CHECK_FALSE(info.size);
    CHECK_FALSE(info.path);
    CHECK_FALSE(info.error);

    auto g = [&](std::string_view qid) {
        std::latch latch(1);
        mgr.getAsset(std::string(qid),
            [&](std::string_view id, const ac::AssetInfo& data) {
                CHECK(id == qid);
                info = data;
                latch.count_down();
            },
            [&](std::string_view id, float f) {
                CHECK(id == qid);
                CHECK(f == 7);
            }
        );
        latch.wait();
    };

    g("no-such-asset");
    CHECK_FALSE(info.source);
    CHECK_FALSE(info.size);
    CHECK_FALSE(info.path);
    CHECK(info.error == "Can't get asset. No source");

    g(Test_Local_Asset_Id);
    REQUIRE(info.source);
    CHECK(info.source->id() == "local-dir: " + Test_Local_Asset_Source_Path);
    CHECK(info.size == Test_Local_Asset_Size);
    CHECK(info.path == Test_Local_Asset_Source_Path + "/" + Test_Local_Asset_Id);
    CHECK_FALSE(info.error);

    g(Another_Test_Asset_Id);
    REQUIRE(info.source);
    CHECK(info.source->id() == "dummy"); // smaller prio
    CHECK(info.size == Dummy_Another_Test_File_Size);
    CHECK(info.path == "dl/" + Another_Test_Asset_Id);
    CHECK_FALSE(info.error);

    g("local-asset");
    REQUIRE(info.source);
    CHECK(info.source->id() == "dummy");
    CHECK(info.size == 1'000'000 + 11);
    CHECK(info.path == "loc/local-asset");
    CHECK_FALSE(info.error);

    g("sizeless-asset-x");
    REQUIRE(info.source);
    CHECK(info.source->id() == "dummy");
    CHECK(info.size == 10'000 + 16);
    CHECK(info.path == "dl/sizeless-asset-x");
    CHECK_FALSE(info.error);

    g("error-asset");
    REQUIRE(info.source);
    CHECK(info.source->id() == "dummy");
    CHECK(info.size == 1000 + 11);
    CHECK_FALSE(info.path);
    CHECK(info.error == "dummy error");
}

TEST_CASE("dir-dummy") {
    ac::AssetManager mgr;

    mgr.addSource(ac::AssetSourceLocalDir_Create(Test_Local_Asset_Source_Path), -10);
    mgr.addSource(std::make_unique<DummyAssetSource>());

    ac::AssetInfo info;
    std::latch latch(1);
    mgr.queryAsset(std::string(Test_Local_Asset_Id), [&](std::string_view id, const ac::AssetInfo& data) {
        CHECK(id == Test_Local_Asset_Id);
        info = data;
        latch.count_down();
    });
    latch.wait();

    REQUIRE(info.source);
    CHECK(info.source->id() == "local-dir: " + Test_Local_Asset_Source_Path);
    CHECK(info.size);
    CHECK(info.path == Test_Local_Asset_Source_Path + "/" + Test_Local_Asset_Id);
    CHECK_FALSE(info.error);
}
