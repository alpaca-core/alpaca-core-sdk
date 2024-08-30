// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <coroutine>
#include <deque>

// a lock for spliced coroutines

namespace astl {

struct coro_lock {
    bool m_locked = false;
    std::deque<std::coroutine_handle<>> m_waiters;
public:
    coro_lock() = default;
    coro_lock(const coro_lock&) = delete;
    coro_lock& operator=(const coro_lock&) = delete;

    bool try_lock() {
        if (!m_locked) {
            m_locked = true;
            return true;
        }
        return false;
    }

    void unlock() {
        if (m_waiters.empty()) {
            m_locked = false;
            return;
        }

        auto h = m_waiters.front();
        m_waiters.pop_front();
        h.resume();
    }

    auto operator co_await() {
        struct guard {
            coro_lock& m_lock;
            ~guard() {
                m_lock.unlock();
            }
        };
        struct awaiter {
            coro_lock& m_lock;
            bool await_ready() const {
                return m_lock.try_lock();
            }
            void await_suspend(std::coroutine_handle<> h) {
                m_lock.m_waiters.push_back(h);
            }
            guard await_resume() {
                return guard{m_lock};
            }
        };
        return awaiter{*this};
    }
};

} // namespace astl
