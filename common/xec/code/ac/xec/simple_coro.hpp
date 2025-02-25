// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <coroutine>

namespace ac::xec {

struct simple_coro {
    struct promise_type;
    using handle = std::coroutine_handle<promise_type>;

    struct promise_type {
        simple_coro get_return_object() {
            return simple_coro(handle::from_promise(*this));
        }
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() noexcept {}
        void unhandled_exception() noexcept { std::terminate(); }
    };

    simple_coro(handle h) noexcept : m_handle(h) {}

    handle m_handle;
};

template <typename Executor>
struct attach_self {
    Executor& m_executor;
    attach_self(Executor& ex) : m_executor(ex) {}
    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> h) noexcept {
        post(m_executor, [h] { h.resume(); });
    }
    void await_resume() noexcept {}
};

} // namespace ac::xec
