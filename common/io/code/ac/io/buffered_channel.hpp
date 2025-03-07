// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "stream_result.hpp"
#include "concepts/stream.hpp"
#include <deque>
#include <mutex>
#include <cassert>

namespace ac::io {

template <typename T>
class buffered_channel {
public:
    using value_type = T;
    using read_value_type = T;
    using write_value_type = T;

    buffered_channel(size_t max_size)
        : m_max_size(max_size)
    {}

    template <on_blocked_cb_class OnBlocked>
    xec::task exchange_read_notify_cb(OnBlocked&& on_blocked) {
        std::lock_guard lock(m_mutex);
        if (m_queue.empty()) {
            return exchange_notify_l(std::forward<OnBlocked>(on_blocked));
        }
        else {
            return nullptr;
        }
    }

    template <on_blocked_cb_class OnBlocked>
    xec::task exchange_write_notify_cb(OnBlocked&& on_blocked) {
        std::lock_guard lock(m_mutex);
        if (m_queue.size() == m_max_size) {
            return exchange_notify_l(std::forward<OnBlocked>(on_blocked));
        }
        else {
            return nullptr;
        }
    }

    template <on_blocked_cb_class OnBlocked>
    stream_result write(T& t, OnBlocked&& on_blocked) {
        std::unique_lock lock(m_mutex);
        if (m_closed) {
            assert(!m_notify);
            return stream_result::build().set_closed();
        }
        else if (m_queue.size() < m_max_size) {
            assert(m_queue.empty() || !m_notify);
            m_queue.push_back(std::move(t));
            unlock_notify(lock);
            return stream_result::build().set_success();
        }
        else {
            return block(std::forward<OnBlocked>(on_blocked));
        }
    }

    template <on_blocked_cb_class OnBlocked>
    stream_result read(T& t, OnBlocked&& on_blocked) {
        std::unique_lock lock(m_mutex);
        if (!m_queue.empty()) {
            assert(m_queue.size() == m_max_size || !m_notify);
            t = std::move(m_queue.front());
            m_queue.pop_front();
            unlock_notify(lock);
            return stream_result::build().set_success();
        }
        else if (m_closed) {
            assert(!m_notify);
            return stream_result::build().set_closed();
        }
        else {
            return block(std::forward<OnBlocked>(on_blocked));
        }
    }

    void close() {
        std::unique_lock lock(m_mutex);
        m_closed = true;
        unlock_notify(lock);
    }
private:
    template <typename OnBlocked>
    xec::task exchange_notify_l(OnBlocked&& on_blocked) {
        if constexpr (std::is_constructible_v<bool, OnBlocked>) {
            if (!on_blocked) {
                return std::exchange(m_notify, nullptr);
            }
        }
        return std::exchange(m_notify, on_blocked());
    }

    xec::task exchange_notify_l(std::nullptr_t) {
        return std::exchange(m_notify, nullptr);
    }

    template <typename OnBlocked>
    stream_result block(OnBlocked&& on_blocked) {
        stream_result ret(exchange_notify_l(std::forward<OnBlocked>(on_blocked)));
        if (ret.value) {
            ret.set_aborted();
        }
        if (m_notify) {
            ret.set_waiting();
        }
        return ret;
    }

    void unlock_notify(std::unique_lock<std::mutex>& lock) {
        assert(lock.owns_lock());
        auto notify = exchange_notify_l(nullptr);
        lock.unlock();

        if (notify) {
            notify();
        }
    }

    const size_t m_max_size;
    std::mutex m_mutex;
    std::deque<T> m_queue; // bTODO: ring buffer
    xec::task m_notify = nullptr;
    bool m_closed = false;
};

} // namespace ac::frameio
