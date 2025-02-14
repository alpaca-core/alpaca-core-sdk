// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/io/status.hpp>
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
