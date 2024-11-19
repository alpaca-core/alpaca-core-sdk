// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <astl/tuple_util.hpp>
#include <doctest/doctest.h>

TEST_CASE("type_index") {
    using namespace astl::tuple;

    using t = std::tuple<int, float, double, float>;

    static_assert(type_index_v<int, t> == 0);
    static_assert(type_index_v<float, t> == 1);
    static_assert(type_index_v<double, t> == 2);
    static_assert(type_index_v<char, t> == -1);
}
