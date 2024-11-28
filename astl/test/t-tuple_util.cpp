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

struct to_int {
    template <typename T>
    constexpr int operator()(T t) const {
        return static_cast<int>(t);
    }

    constexpr int operator()() {
        return -45;
    }
};

TEST_CASE("index_switch") {
    std::tuple t = {1, 2.3f, 3.2, 4.1f};

    auto do_switch = [&](int n) {
        return astl::tuple::switch_index(t, n, to_int{});
    };

    CHECK(do_switch(0) == 1);
    CHECK(do_switch(1) == 2);
    CHECK(do_switch(2) == 3);
    CHECK(do_switch(3) == 4);
    CHECK(do_switch(4212) == -45);
    CHECK(do_switch(-3) == -45);
}

TEST_CASE("type_switch") {
    constexpr std::tuple t = { 1, 2.3f, 3.2, 4.1f };

    static_assert(astl::tuple::switch_type<int>(t, to_int{}) == 1);
    static_assert(astl::tuple::switch_type<float>(t, to_int{}) == 2);
    static_assert(astl::tuple::switch_type<double>(t, to_int{}) == 3);
    static_assert(astl::tuple::switch_type<char>(t, to_int{}) == -45);
}