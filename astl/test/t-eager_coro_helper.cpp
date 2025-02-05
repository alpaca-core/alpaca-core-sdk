// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <astl/eager_coro_helper.hpp>
#include <doctest/doctest.h>
#include <vector>
#include <string>

struct eager_wrapper {
    struct promise_type : public astl::eager_coro_helper::promise {
        eager_wrapper get_return_object() {
            return eager_wrapper{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() noexcept { return {}; } // eager
        std::suspend_always final_suspend() noexcept { return {}; } // preserve the final yield
        void return_void() noexcept {}
        using astl::eager_coro_helper::promise::unhandled_exception;

        int last_value = -1;
        std::suspend_always yield_value(int val) {
            last_value = val;
            on_suspend();
            return {};
        }
    };

    std::coroutine_handle<promise_type> handle = nullptr;

    explicit eager_wrapper(std::coroutine_handle<promise_type> h = nullptr) noexcept : handle(h) {}
    ~eager_wrapper() noexcept {
        astl::eager_coro_helper::safe_destroy_handle(handle);
    }

    int get() {
        if (!handle || handle.done()) return -1;
        auto ret = handle.promise().last_value;
        handle.resume();
        handle.promise().rethrow_if_exception();
        return ret;
    }
};

eager_wrapper test_coro(int from, int to, int throw_on = -1) {
    // fill vector so as to catch double delete issues from clang
    std::vector<int> v;
    for (int i = from; i < to; ++i) {
        v.push_back(i);
    }

    for (int i = from; i < to; ++i) {
        if (i == throw_on) {
            throw std::runtime_error(std::to_string(i));
        }
        co_yield v[i];
    }
}

TEST_CASE("no throw") {
    auto gen = test_coro(0, 10);
    for (int i = 0; i < 10; ++i) {
        CHECK(gen.get() == i);
    }
    CHECK(gen.get() == -1);
}

TEST_CASE("eager throw") {
    CHECK_THROWS_WITH(test_coro(0, 10, 0), "0");
}

TEST_CASE("lazy throw") {
    auto gen = test_coro(0, 10, 5);
    for (int i = 0; i < 4; ++i) {
        CHECK(gen.get() == i);
    }
    CHECK_THROWS_WITH(gen.get(), "5");
}
