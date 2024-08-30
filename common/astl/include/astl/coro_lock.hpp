// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <coroutine>
#include <utility>
#include <deque>

// a lock for spliced coroutines

namespace astl {

struct coro_lock {
    bool m_locked = false;
    std::deque<std::coroutine_handle<>> m_awaiters;
public:
    coro_lock() = default;
    coro_lock(const coro_lock&) = delete;
    coro_lock& operator=(const coro_lock&) = delete;

    bool try_lock() noexcept {
        if (!m_locked) {
            m_locked = true;
            return true;
        }
        return false;
    }

    void drop_awaiter(std::coroutine_handle<> aw) {
        // instead of removing the awaiter from the list, we just null it out
        for (auto& w : m_awaiters) {
            if (w != aw) continue;

            w = nullptr;
            return;
        }
        // assert false here? we shouldn't be dropping an awaiter that doesn't exist
    }

    void unlock() {
        while (!m_awaiters.empty()) {
            auto h = m_awaiters.front();
            m_awaiters.pop_front();
            if (!h) continue; // disregard dropped awaiters
            h.resume();
            return;
        }

        m_locked = false;
        return;
    }

    bool locked() const noexcept {
        return m_locked;
    }

    class guard {
        coro_lock* m_lock = nullptr;
    public:
        guard() noexcept = default;
        explicit guard(coro_lock& lock) noexcept : m_lock(&lock) {}
        ~guard() {
            unlock();
        }

        guard(const guard&) = delete;
        guard& operator=(const guard&) = delete;
        guard(guard&& other) noexcept : m_lock(std::exchange(other.m_lock, nullptr)) {}
        guard& operator=(guard&& other) noexcept {
            if (this != &other) {
                unlock();
                m_lock = std::exchange(other.m_lock, nullptr);
            }
            return *this;
        }

        void unlock() {
            if (m_lock) {
                m_lock->unlock();
                m_lock = nullptr;
            }
        }

        explicit operator bool() const noexcept {
            return m_lock;
        }
    };

    auto operator co_await() {
        struct awaitable {
            coro_lock& m_lock;
            std::coroutine_handle<> m_handle;
            bool await_ready() const noexcept {
                return m_lock.try_lock();
            }
            void await_suspend(std::coroutine_handle<> h) {
                m_handle = h;
                m_lock.m_awaiters.push_back(h);
            }
            guard await_resume() noexcept {
                m_handle = nullptr;
                return guard{m_lock};
            }
            ~awaitable() {
                if (!m_handle) return;

                // well, this is unpleasant
                // the coroutine was destroyed while awaiting this
                // this means it shouldn't be resumed, so clear it from the awaiters
                m_lock.drop_awaiter(m_handle);
            }
        };
        return awaitable{*this};
    }
};

} // namespace astl
