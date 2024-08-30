// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <astl/coro_lock.hpp>
#include <doctest/doctest.h>

TEST_CASE("no coro") {
    astl::coro_lock lock;
    CHECK_FALSE(lock.locked());
    CHECK(lock.try_lock());
    CHECK(lock.locked());
    CHECK_FALSE(lock.try_lock());
    CHECK_FALSE(lock.try_lock_guard());
    lock.unlock();
    CHECK_FALSE(lock.locked());
    CHECK(lock.try_lock());
    CHECK(lock.locked());

    lock.unlock();
    {
        auto g = lock.try_lock_guard();
        CHECK(!!g);
        CHECK(lock.locked());
        CHECK_FALSE(lock.try_lock_guard());
    }
    CHECK_FALSE(lock.locked());
}

class coro {
public:
    struct promise_type {
        coro get_return_object() noexcept {
            return coro{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        void unhandled_exception() noexcept { /* terminate? */ }
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }

        int value = -1;
        void return_value(int v) {
            value = v;
        }
    };

    int value() const {
        return m_handle.promise().value;
    }

    void resume() {
        m_handle.resume();
    }

    ~coro() {
        m_handle.destroy();
    }

private:
    using handle = std::coroutine_handle<promise_type>;
    handle m_handle;
    explicit coro(handle handle) noexcept : m_handle(handle) {}
};

coro test_await(astl::coro_lock& lock, int i) {
    auto l = co_await lock;
    CHECK(lock.locked()); // must have lock here
    co_return i;
}

coro test_await_suspend(astl::coro_lock& lock, int i) {
    auto l = co_await lock;
    co_await std::suspend_always{};
    CHECK(lock.locked()); // must have lock here
    co_return i;
}

coro test_maybe_await(astl::coro_lock& lock, int i, bool b) {
    astl::coro_lock::guard g;
    if (b) {
        g = co_await lock;
        co_await std::suspend_always{};
    }
    CHECK(b == lock.locked());
    CHECK(b == static_cast<bool>(g));
    co_return i;
}

TEST_CASE("release from completed coroutine") {
    astl::coro_lock lock;
    auto c = test_await(lock, 42);
    CHECK(c.value() == 42); // completed
    CHECK_FALSE(lock.locked()); // released
}

TEST_CASE("held by suspended coroutine, but released upon destruction") {
    astl::coro_lock lock;

    {
        auto c = test_await_suspend(lock, 42);
        CHECK(lock.locked());
        CHECK(c.value() == -1); // suspended
    }

    CHECK_FALSE(lock.locked()); // guard was destroyed when c was destroyed
}

TEST_CASE("maybe suspend") {
    astl::coro_lock lock;

    auto c = test_maybe_await(lock, 42, false);
    CHECK_FALSE(lock.locked());
    CHECK(c.value() == 42);

    auto d = test_maybe_await(lock, 43, true);
    CHECK(lock.locked());
    CHECK(d.value() == -1);
    d.resume();
    CHECK(d.value() == 43);
}

TEST_CASE("released by resumed coroutine") {
    astl::coro_lock lock;

    auto c = test_await_suspend(lock, 34);
    CHECK(lock.locked());
    CHECK(c.value() == -1); // suspended
    c.resume();
    CHECK(c.value() == 34);
    CHECK_FALSE(lock.locked());
}

TEST_CASE("safely dropping awaiters") {
    astl::coro_lock lock;

    {
        auto c = test_await_suspend(lock, 34);
        CHECK(lock.locked());
        // next coros should be suspended on lock
        auto d0 = test_await(lock, 1);
        CHECK(d0.value() == -1);
        auto d1 = test_await(lock, 2);
        CHECK(d1.value() == -1);
        auto d2 = test_await_suspend(lock, 3);
        CHECK(d2.value() == -1);
    }

    CHECK_FALSE(lock.locked());
}

TEST_CASE("splicing") {

    astl::coro_lock lock;
    auto c = test_await_suspend(lock, 34);
    CHECK(lock.locked());
    auto a0 = test_await(lock, 0);
    CHECK(a0.value() == -1); // suspended on lock
    auto a1 = test_await(lock, 1);
    CHECK(a1.value() == -1);
    auto s2 = test_maybe_await(lock, 2, true);
    CHECK(s2.value() == -1);
    auto b3 = test_await(lock, 3); // blocked by s2
    CHECK(b3.value() == -1);

    c.resume();
    CHECK(c.value() == 34);

    // which should have unleashed 0 and 1
    CHECK(a0.value() == 0);
    CHECK(a1.value() == 1);

    // but stopped on s2
    CHECK(lock.locked());
    CHECK(s2.value() == -1);
    CHECK(b3.value() == -1);

    s2.resume();
    CHECK_FALSE(lock.locked());

    CHECK(s2.value() == 2);
    CHECK(b3.value() == 3);
}
