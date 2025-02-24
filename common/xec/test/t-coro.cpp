// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/xec/coro.hpp>
#include <ac/xec/timer_wobj.hpp>
#include <ac/xec/strand_wobj.hpp>
#include <ac/xec/context.hpp>
#include <doctest/doctest.h>

using namespace ac::xec;

coro<void> trivial() { co_return; }

TEST_CASE("trivial") {
    context ctx;
    co_spawn(ctx, trivial());
    ctx.run();
}

coro<int> five() { co_return 5; }
coro<int> seven() { co_return 7; }

coro<void> twelve(int& result) {
    result = co_await five() + co_await seven();
}

TEST_CASE("await sum") {
    context ctx;
    int result = 0;
    co_spawn(ctx, twelve(result));
    ctx.run();
    CHECK(result == 12);
}

template <typename Wobj>
coro<void> notify(Wobj& wobj) {
    wobj.notify_one();
    co_return;
}

coro<void> await_respawn() {
    auto ex = co_await executor{};

    {
        timer_wobj tw(ex);
        co_spawn(ex, notify(tw));
        auto notified = co_await tw.wait(astl::timeout::after_ms(4311));
        CHECK(notified);

        notified = co_await tw.wait(astl::timeout::after_ms(10));
        CHECK(!notified);
    }

    {
        strand_wobj tw(ex);
        co_spawn(ex, notify(tw));
        auto notified = co_await tw.wait();
        CHECK(notified);
    }
}

TEST_CASE("await respawn") {
    context ctx;
    co_spawn(ctx, await_respawn());
    ctx.run();
}

coro<int> maybe_throw(int level, bool t) {
    if (level == 0 && t) {
        throw std::runtime_error("ex");
    }
    if (level == 0) {
        co_return 0;
    }
    co_return 1 + co_await maybe_throw(level - 1, t);
}

coro<void> test_exceptions() {
    CHECK(co_await maybe_throw(0, false) == 0);
    CHECK_THROWS_WITH(co_await maybe_throw(0, true), "ex");

    CHECK(co_await maybe_throw(4, false) == 4);
    CHECK_THROWS_WITH(co_await maybe_throw(4, true), "ex");
}

TEST_CASE("exceptions") {
    context ctx;
    co_spawn(ctx, test_exceptions());
    ctx.run();
}

struct TestCoro : public coro_state {
    using coro_state::coro_state;

    int value = 0;
    int result = 0;

    coro<int> get_value() {
        co_return 10;
    }

    coro<void> entry() {
        result = co_await get_value() + co_await five();
    }
};

TEST_CASE("coro state") {
    context ctx;
    auto state = std::make_shared<TestCoro>(ctx.make_strand());
    state->value = 10;
    co_spawn(state, state->entry());
    ctx.run();
    CHECK(state->result == 15);
}
