// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/FsUtil.hpp>
#include <test-assets/assets.h>
#include <doctest/doctest.h>
#include <cstdlib>

#if defined(_WIN32)
// ::SetEnvironmentVariable doesn't work with std::getenv :(

#include <vector>
#include <string_view>

namespace {
std::vector<std::vector<char>> vars;
void setenv(std::string_view name, std::string_view value, int) {
    std::vector<char> new_var(name.length() + value.length() + 1 /*=*/ + 1 /*0*/);
    char* str = new_var.data();
    memcpy(str, name.data(), name.length());
    str += name.length();
    *str++ = '=';
    memcpy(str, value.data(), value.length());
    str += value.length();
    *str = 0;

    auto& v = vars.emplace_back(std::move(new_var));
    _putenv(v.data());
}
}
#endif

TEST_CASE("expandPath") {
    // can't test home without reinforcing bugs
    {
        auto e = ac::fs::expandPath("~/foo");

        CHECK(e.ends_with("/foo"));

#if defined(_WIN32)
        REQUIRE(e.length() > 5);
        CHECK(e[1] == ':'); // must be absolute
#else
        CHECK(e.length >= 4);
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
