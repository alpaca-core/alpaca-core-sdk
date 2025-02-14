// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <astl/timeout.hpp>
#include <doctest/doctest.h>

TEST_CASE("timeout") {
    using astl::timeout;
    {
        timeout t(std::chrono::milliseconds(42));
        CHECK(t.ms() == 42);
        CHECK_FALSE(t.is_infinite());
        CHECK_FALSE(t.is_zero());
    }

    {
        auto t = timeout::after(std::chrono::milliseconds(12));
        CHECK(t.ms() == 12);
        CHECK_FALSE(t.is_infinite());
        CHECK_FALSE(t.is_zero());
    }

    {
        auto t = timeout::after_ms(8);
        CHECK(t.ms() == 8);
        CHECK_FALSE(t.is_infinite());
        CHECK_FALSE(t.is_zero());
    }

    {
        auto t = timeout::never();
        CHECK(t.ms() < 0);
        CHECK(t.is_infinite());
        CHECK_FALSE(t.is_zero());
    }

    {
        auto t = timeout::now();
        CHECK(t.ms() == 0);
        CHECK_FALSE(t.is_infinite());
        CHECK(t.is_zero());
    }

    {
        auto t = timeout::immediately();
        CHECK(t.ms() == 0);
        CHECK_FALSE(t.is_infinite());
        CHECK(t.is_zero());
    }
}

TEST_CASE("chrono") {
    using namespace std::chrono_literals;
    using astl::timeout;

    timeout t = 42ms;
    CHECK(t.ms() == 42);

    t = timeout::after(34ms);
    CHECK(t.ms() == 34);

    t = timeout::after(2s);
    CHECK(t.ms() == 2000);

    t = timeout::after(-3min);
    CHECK(t.is_infinite());

    t = timeout::after(0ms);
    CHECK(t.is_zero());
}

TEST_CASE("vals") {
    using namespace astl::timeout_vals;
    CHECK(await_completion_for(std::chrono::milliseconds(53)).ms() == 53);
    CHECK(await_completion.is_infinite());
    CHECK(no_wait.is_zero());
    CHECK(proceed_immediately.is_zero());
}
