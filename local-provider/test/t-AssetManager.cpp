// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/AssetSourceLocalDir.hpp>
#include <ac/AssetManager.hpp>
#include "ac-repo-root.h"
#include <doctest/doctest.h>
#include <latch>

class DummyAssetSource : public ac::AssetSource {
public:
    virtual std::string_view id() const {
        return "dummy";
    }

    virtual std::optional<BasicAssetInfo> checkAssetSync(std::string_view id) override {
        if (id.starts_with("VS") || id.starts_with("no")) {
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
    virtual itlib::expected<BasicAssetInfo, std::string> fetchAssetSync(std::string_view id, ProgressCb) override {
        auto basicInfo = checkAssetSync(id);
        if (!basicInfo) {
            return itlib::unexpected("dummy not found");
        }
        if (id.starts_with("error")) {
            return itlib::unexpected("dummy error");
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
    mgr.addSource(ac::AssetSourceLocalDir_Create(AC_REPO_ROOT), 10);
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

    q("VSOpenFileFromDirFilters.json");
    REQUIRE(info.source);
    CHECK(info.source->id() == "local-dir: " AC_REPO_ROOT);
    CHECK(info.size == 86);
    CHECK(info.path == AC_REPO_ROOT "/VSOpenFileFromDirFilters.json");
    CHECK_FALSE(info.error);

    q("CMakeLists.txt");
    REQUIRE(info.source);
    CHECK(info.source->id() == "dummy"); // smaller prio
    CHECK(info.size == 1'000 + 14);
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
        mgr.getAsset(std::string(qid), [&](std::string_view id, const ac::AssetInfo& data) {
            CHECK(id == qid);
            info = data;
            latch.count_down();
        });
        latch.wait();
    };

    g("no-such-asset");
    CHECK_FALSE(info.source);
    CHECK_FALSE(info.size);
    CHECK_FALSE(info.path);
    CHECK(info.error == "Asset not found");

    g("VSOpenFileFromDirFilters.json");
    REQUIRE(info.source);
    CHECK(info.source->id() == "local-dir: " AC_REPO_ROOT);
    CHECK(info.size == 86);
    CHECK(info.path == AC_REPO_ROOT "/VSOpenFileFromDirFilters.json");
    CHECK_FALSE(info.error);

    g("CMakeLists.txt");
    REQUIRE(info.source);
    CHECK(info.source->id() == "dummy"); // smaller prio
    CHECK(info.size == 1'000 + 14);
    CHECK(info.path == "dl/CMakeLists.txt");
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
    mgr.addSource(ac::AssetSourceLocalDir_Create(AC_REPO_ROOT), -10);
    mgr.addSource(std::make_unique<DummyAssetSource>());

    ac::AssetInfo info;
    std::latch latch(1);
    mgr.queryAsset(std::string("CMakeLists.txt"), [&](std::string_view id, const ac::AssetInfo& data) {
        CHECK(id == "CMakeLists.txt");
        info = data;
        latch.count_down();
    });
    latch.wait();

    REQUIRE(info.source);
    CHECK(info.source->id() == "local-dir: " AC_REPO_ROOT);
    CHECK(info.size);
    CHECK(info.path == AC_REPO_ROOT "/CMakeLists.txt");
    CHECK_FALSE(info.error);
}
