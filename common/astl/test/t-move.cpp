// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <doctest/doctest.h>

#include <astl/move.hpp>

#include <string>

TEST_CASE("move") {
    auto i = astl::move(7);
    CHECK(i == 7);
    const auto str = astl::move(std::string("str"));
    CHECK(str == "str");
    // won't compile:
    // auto s2 = astl::move(str);
}
