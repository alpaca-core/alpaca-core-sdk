// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/frameio/Status.hpp>
#include <doctest/doctest.h>

TEST_CASE("Status") {
    using namespace ac::frameio;

    Status s;
    CHECK(s.bits.none());
    CHECK_FALSE(s.success());
    CHECK_FALSE(s.timeout());
    CHECK_FALSE(s.aborted());
    CHECK_FALSE(s.closed());
    CHECK(s.blocked());
    CHECK_FALSE(s.complete());

    s.setSuccess();
    CHECK(s.bits.count() == 1);
    CHECK(s.success());
    CHECK_FALSE(s.blocked());
    CHECK(s.complete());

    s.reset();
    CHECK(s.bits.none());

    s.setAborted();
    CHECK(s.bits.count() == 1);
    CHECK(s.aborted());
    CHECK_FALSE(s.blocked());
    CHECK_FALSE(s.complete());

    s.setClosed();
    CHECK(s.bits.count() == 2);
    CHECK(s.aborted());
    CHECK(s.closed());
    CHECK_FALSE(s.blocked());
    CHECK(s.complete());

    s.setBlocked();
    CHECK(s.bits.none());

    s.setClosed();
    CHECK(s.bits.count() == 1);
    CHECK(s.complete());

    s.reset();
    s.setTimeout();
    CHECK(s.bits.count() == 1);
    CHECK(s.timeout());
    CHECK_FALSE(s.blocked());
    CHECK_FALSE(s.complete());

    s.setSuccess();
    CHECK(s.bits.count() == 2);
    CHECK(s.success());
    CHECK_FALSE(s.blocked());
    CHECK(s.complete());
}
