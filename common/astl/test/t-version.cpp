// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <astl/version.hpp>
#include <doctest/doctest.h>

TEST_CASE("version") {
    constexpr astl::version v1{1, 2, 3};
    static_assert(v1.major == 1);
    static_assert(v1.minor == 2);
    static_assert(v1.patch == 3);
    static_assert(v1.to_int() == 1'002'003);
    static_assert(astl::version::from_int(1'002'003) == v1);

    constexpr astl::version v2{431, 252, 508};
    static_assert(v2.major == 431);
    static_assert(v2.minor == 252);
    static_assert(v2.patch == 508);
    static_assert(v2.to_int() == 431'252'508);
    static_assert(astl::version::from_int(431'252'508) == v2);

    static_assert(v1 < v2);
    static_assert(v2 > v1);

    astl::version v3 = v1;
    CHECK(v3 == v1);
    CHECK(v3 < v2);
    CHECK(v2 > v3);

    v3.patch = 5;
    CHECK(v3.to_int() == 1'002'005);
    CHECK(v3 > v1);
    v3 = v1;
    v3.minor = 5;
    CHECK(v3.to_int() == 1'005'003);
    CHECK(v3 > v1);
    v3 = v1;
    v3.major = 5;
    CHECK(v3.to_int() == 5'002'003);
    CHECK(v3 > v1);
}

