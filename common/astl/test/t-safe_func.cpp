// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <astl/safe_func.hpp>
#include <functional>
#include <doctest/doctest.h>

void foo(int& c, int a, int b) {
    c = a + b;
}

TEST_CASE("raw") {
    astl::safe_func f = foo;
    int c = 0;
    f(c, 1, 2);
    CHECK(c == 3);

    f = {};
    f(c, 10, 20);
    CHECK(c == 3);
}

TEST_CASE("lambda") {
    astl::safe_func f = [](int& c, int a, int b) { c = a * b; };
    int c = 0;
    f(c, 3, 2);
    CHECK(c == 6);
}

TEST_CASE("std::function") {
    astl::safe_func<std::function<void(int&, int, int)>> f{[](int& c, int a, int b) { c = a / b; }};
    int c = 0;
    f(c, 3, 2);
    CHECK(c == 1);

    f = {};
    f(c, 20, 2);
    CHECK(c == 1);

    f = [](int& c, int a, int b) { c = a * b; };
    f(c, 3, 2);
    CHECK(c == 6);
}
