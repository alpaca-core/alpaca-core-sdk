// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "t-sv.h"
#include <astl/sv.h>
#include <doctest/doctest.h>

TEST_CASE("ac_sv") {
    ac_sv a = {};
    CHECK_FALSE(a.begin);
    CHECK_FALSE(a.end);
    CHECK(a.empty());

    std::string_view sv = a.to_std();
    CHECK(sv.empty());

    CHECK(a == "");
    CHECK(a == sv);
    CHECK(a != "x");

    a = ac_sv::from_std("asdf");
    CHECK(a == "asdf");
    CHECK(a != "a");
    CHECK(sv != a);

    sv = "asdf";
    CHECK(sv == a);

    ac_sv b = ac_sv::from_std("asdf");
    CHECK(a == b);

    b = ac_sv::from_std("azdf");
    CHECK(a < b);
    CHECK(a != b);
}

extern "C" {
    bool c_to_cxx(ac_sv* a, ac_sv* b) {
        auto ret = *a == *b;
        *a = ac_sv::from_std("c++ here");
        *b = ac_sv::from_std("and again here");
        return ret;
    }
}

TEST_CASE("cross") {
    ac_sv a = ac_sv::from_std("it's c++ calling");
    ac_sv b = a;
    CHECK(cxx_to_c(&a, &b));
    CHECK(a == "t's c++ callin");
    CHECK(b == "hello from c");
}
