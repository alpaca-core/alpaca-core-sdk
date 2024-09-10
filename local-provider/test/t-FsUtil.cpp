// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/FsUtil.hpp>
#include <test-assets/assets.h>
#include <doctest/doctest.h>

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
