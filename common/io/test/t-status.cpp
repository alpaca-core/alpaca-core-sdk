// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/io/value_with_status.hpp>
#include <doctest/doctest.h>

TEST_CASE("status") {
    ac::io::status s;
    CHECK(s.bits.none());
    CHECK_FALSE(s.success());
    CHECK_FALSE(s.timeout());
    CHECK_FALSE(s.aborted());
    CHECK_FALSE(s.closed());
    CHECK(s.blocked());
    CHECK_FALSE(s.complete());

    s.set_success();
    CHECK(s.bits.count() == 1);
    CHECK(s.success());
    CHECK_FALSE(s.blocked());
    CHECK(s.complete());

    s.reset();
    CHECK(s.bits.none());

    s.set_aborted();
    CHECK(s.bits.count() == 1);
    CHECK(s.aborted());
    CHECK(s.blocked());
    CHECK_FALSE(s.complete());

    s.set_closed();
    CHECK(s.bits.count() == 2);
    CHECK(s.aborted());
    CHECK(s.closed());
    CHECK_FALSE(s.blocked());
    CHECK(s.complete());

    s.reset();

    s.set_closed();
    CHECK(s.bits.count() == 1);
    CHECK(s.complete());

    s.reset();
    s.set_timeout();
    CHECK(s.bits.count() == 1);
    CHECK(s.timeout());
    CHECK(s.blocked());
    CHECK_FALSE(s.complete());

    s.set_success();
    CHECK(s.bits.count() == 2);
    CHECK(s.success());
    CHECK_FALSE(s.blocked());
    CHECK(s.complete());
}

TEST_CASE("value with status") {
    ac::io::value_with_status<int> v;
    CHECK(v.value == 0);
    v.value = 432;
    v.reset();
    CHECK(v.value == 0);
    v.reset(42);
    CHECK(v.value == 42);

    ac::io::status& s = v;
    s.set_aborted();
    CHECK(v.aborted());

    ac::io::value_with_status<int> v2(42);
    CHECK(v2.value == 42);

    v2.s() = s;
    CHECK(v2.aborted());
}

ac::io::value_with_status<int> func() {
    return ac::io::value_with_status<int>::build(213).set_aborted().set_closed();
}

TEST_CASE("value with status builder") {
    auto v = func();
    CHECK(v.value == 213);
    CHECK(v.aborted());
    CHECK(v.closed());
}
