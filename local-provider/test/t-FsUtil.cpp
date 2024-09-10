// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/FsUtil.hpp>
#include <test-assets/assets.h>
#include <ac-test-util/setenv.hpp>
#include <doctest/doctest.h>

TEST_CASE("expandPath") {
    // can't test home without reinforcing bugs
    {
        auto e = ac::fs::expandPath("~/foo");

        CHECK(e.ends_with("/foo"));

#if defined(_WIN32)
        REQUIRE(e.length() > 5);
        CHECK(e[1] == ':'); // must be absolute
#else
        CHECK(e.length() >= 4);
        CHECK(e[0] == '/'); // must be absolute
#endif
    }

    auto test = [](const std::string& path, std::string_view expected) {
        auto expanded = ac::fs::expandPath(path);
        CHECK(expanded == expected);
        auto inplace = path;
        ac::fs::expandPathInPlace(inplace);
        CHECK(inplace == expected);
    };

    setenv("AC_TEST_VAR", "foo", 1);
    test("$AC_TEST_VAR", "foo");
    test("$AC_TEST_VAR/bar", "foo/bar");

    CHECK_THROWS_WITH_AS(ac::fs::expandPath("$nope42/xxx"), "Environment variable nope42 not set", std::runtime_error);
}

const std::string binDir = TEST_ASSETS_BINARY_PATH;
const std::string bin1 = TEST_ASSETS_BINARY_PATH "/" TA_BINARY_FILE;
const std::string bin2 = TEST_ASSETS_BINARY_PATH "/" TA_ANOTHER_BINARY_FILE;
const std::string nope = TEST_ASSETS_BINARY_PATH "/no-such-file";

TEST_CASE("stat") {
    auto s = ac::fs::basicStat(binDir);
    CHECK(s.exists());
    CHECK(s.dir());
    CHECK(!s.file());
    CHECK(s.size == 0);

    s = ac::fs::basicStat(bin1);
    CHECK(s.exists());
    CHECK(s.file());
    CHECK(!s.dir());
    CHECK(s.size == TA_BINARY_FILE_SIZE);

    s = ac::fs::basicStat(bin2);
    CHECK(s.exists());
    CHECK(s.file());
    CHECK(!s.dir());
    CHECK(s.size == TA_ANOTHER_BINARY_FILE_SIZE);

    s = ac::fs::basicStat(nope);
    CHECK(!s.exists());
    CHECK(!s.file());
    CHECK(!s.dir());
    CHECK(s.size == 0);
}
