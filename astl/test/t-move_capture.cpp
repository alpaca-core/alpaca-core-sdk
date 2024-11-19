// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <astl/move_capture.hpp>
#include <doctest/doctest.h>
#include <string>
#include <vector>

TEST_CASE("move capture macro ret") {
    std::vector<int> v1{ 1, 2, 3 };
    std::vector<std::string> v2{ "a", "b", "c" };
    auto v2data = v2.data();
    auto v1data = v1.data();
    auto ret_lambda = [movecap(v1, v2)]() {
        return std::make_pair(v1.data(), v2.data());
    };
    CHECK(v1.empty());
    CHECK(v2.empty());
    auto [v1data_ret, v2data_ret] = ret_lambda();
    CHECK(v1data == v1data_ret);
    CHECK(v2data == v2data_ret);
}

TEST_CASE("move capture macro check") {
    std::vector<int> v1{ 1, 2, 3 };
    std::vector<std::string> v2{ "a", "b", "c" };
    auto v2data = v2.data();
    auto v1data = v1.data();
    // check that additional captures can be added
    auto check_lambda = [&v1data, movecap(v1, v2), v2data]() {
        CHECK(v1.data() == v1data);
        CHECK(v2.data() == v2data);
    };
    CHECK(v1.empty());
    CHECK(v2.empty());
    check_lambda();
}
