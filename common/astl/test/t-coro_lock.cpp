// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <astl/coro_lock.hpp>
#include <doctest/doctest.h>

TEST_CASE("trivial") {
    astl::coro_lock lock;
    CHECK_FALSE(lock.locked());
    CHECK(lock.try_lock());
    CHECK(lock.locked());
    CHECK_FALSE(lock.try_lock());
    lock.unlock();
    CHECK_FALSE(lock.locked());
    CHECK(lock.try_lock());
    CHECK(lock.locked());
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

coro test_await(astl::coro_lock& lock, int i, bool suspend_after_lock) {
    auto l = co_await lock;
    if (suspend_after_lock) {
        co_await std::suspend_always{};
    }
    CHECK(lock.locked()); // must have lock here
    co_return i;
}

TEST_CASE("release from completed coroutine") {
    astl::coro_lock lock;
    auto c = test_await(lock, 42, false);
    CHECK(c.value() == 42); // completed
    CHECK_FALSE(lock.locked()); // released
}

TEST_CASE("held by incomplete coroutine, but released upon destruction") {
    astl::coro_lock lock;

    {
        auto c = test_await(lock, 42, true);
        CHECK(lock.locked());
        CHECK(c.value() == -1); // suspended
    }

    CHECK_FALSE(lock.locked()); // guard was destroyed when c was destroyed
}

TEST_CASE("released by resumed coroutine") {
    astl::coro_lock lock;

    auto c = test_await(lock, 34, true);
    CHECK(lock.locked());
    CHECK(c.value() == -1); // suspended
    c.resume();
    CHECK(c.value() == 34);
    CHECK_FALSE(lock.locked());
}

TEST_CASE("safely dropping awaiters") {
    astl::coro_lock lock;

    {
        auto c = test_await(lock, 34, true);
        CHECK(lock.locked());
        // next coros should be suspended on lock
        auto d0 = test_await(lock, 1, false);
        CHECK(d0.value() == -1);
        auto d1 = test_await(lock, 2, false);
        CHECK(d1.value() == -1);
        auto d2 = test_await(lock, 3, true);
        CHECK(d2.value() == -1);
    }

    CHECK_FALSE(lock.locked());
}

TEST_CASE("splicing") {

    astl::coro_lock lock;
    auto c = test_await(lock, 34, true);
    CHECK(lock.locked());
    auto a0 = test_await(lock, 0, false);
    CHECK(a0.value() == -1); // suspended on lock
    auto a1 = test_await(lock, 1, false);
    CHECK(a1.value() == -1);
    auto s2 = test_await(lock, 2, true); // suspend
    CHECK(s2.value() == -1);
    auto b3 = test_await(lock, 3, false); // blocked by s2
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