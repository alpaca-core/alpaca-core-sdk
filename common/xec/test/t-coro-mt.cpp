// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/xec/coro.hpp>
#include <ac/xec/strand_wobj.hpp>
#include <ac/xec/timer_wobj.hpp>
#include <ac/xec/context.hpp>
#include <ac/xec/context_work_guard.hpp>
#include <ac/xec/co_spawn.hpp>
#include <ac/xec/co_execute.hpp>
#include <ac/xec/thread_runner.hpp>
#include <doctest/doctest.h>

class worker {
    ac::xec::timer_wobj m_wobj;
    ac::xec::strand_wobj& m_notify;
public:
    worker(ac::xec::strand s, ac::xec::strand_wobj& notify)
        : m_wobj(s)
        , m_notify(notify)
    {
        co_spawn(s, run());
    }

    int a, b;
    int result;
    ac::xec::coro<void> run() {
        while (true) {
            auto notified = co_await m_wobj.wait(astl::timeout::after_ms(100));
            if (!notified) continue;
            result = a + b;
            m_notify.notify_one();
            if (result == 0) co_return;
        }
    }

    void notify() {
        m_wobj.notify_one();
    }
};

ac::xec::coro<int> test() {
    ac::xec::context ctx;
    auto wg = ctx.make_work_guard();
    ac::xec::thread_runner runner(ctx, 1, "worker");

    ac::xec::strand_wobj wobj(co_await ac::xec::executor{});
    worker wrk(ctx.make_strand(), wobj);

    wrk.a = 5;
    wrk.b = 10;
    wrk.notify();
    CHECK(co_await wobj.wait());
    CHECK(wrk.result == 15);

    wrk.a = 20;
    wrk.b = 30;
    wrk.notify();
    CHECK(co_await wobj.wait());
    CHECK(wrk.result == 50);

    wrk.a = 0;
    wrk.b = 0;
    wrk.notify();
    CHECK(co_await wobj.wait());

    wg.reset();
    co_return wrk.result;
}

TEST_CASE("worker") {
    CHECK(co_execute(test()) == 0);
}
