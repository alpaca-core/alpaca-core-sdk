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

TEST_CASE("index_switch") {
    std::tuple t = {1, 2.3f, 3.2, 4.1f};

    auto do_switch = [&](int n) {
        return astl::tuple::switch_index(t, n,
            [](auto& v) { return static_cast<int>(v); },
            [] { return -45; }
        );
    };

    CHECK(do_switch(0) == 1);
    CHECK(do_switch(1) == 2);
    CHECK(do_switch(2) == 3);
    CHECK(do_switch(3) == 4);
    CHECK(do_switch(4212) == -45);
    CHECK(do_switch(-3) == -45);
}

struct identity {
    template <typename T>
    constexpr T& operator()(T& t) const {
        return t;
    }
    int& operator()(nullptr_t) const {
        throw 0;
    }
};

TEST_CASE("ref") {
    std::tuple t = {1, 2, 3, 4};

    astl::tuple::switch_index(t, 2,
        std::identity{},
        []() -> int& { throw 0; }
    ) = 42;
    CHECK(std::get<2>(t) == 42);
}
