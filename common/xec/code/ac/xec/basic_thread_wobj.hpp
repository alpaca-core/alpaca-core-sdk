// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "wait_func_invoke.hpp"
#include <condition_variable>
#include <atomic>
#include <mutex>

namespace ac::xec {

class basic_thread_wobj {
    alignas(std::hardware_destructive_interference_size)
        std::atomic_bool m_flag;

    std::mutex m_mutex;
    std::condition_variable m_cvar;

    bool wait_pred() {
        return m_flag.exchange(false, std::memory_order_acquire);
    }

    template <typename Cb>
    void call_cb(Cb& cb, bool notified) {
        if (notified) {
            wait_func_invoke_cancelled(cb);
        }
        else {
            wait_func_invoke_timeout(cb);
        }
    }
public:
    void notify_all() {
        m_flag.store(true, std::memory_order_release);
        m_cvar.notify_all();
    }

    void notify_one() {
        m_flag.store(true, std::memory_order_release);
        m_cvar.notify_one();
    }

    template <typename Cb>
    void wait(Cb&& cb) {
        {
            std::unique_lock lock(m_mutex);
            m_cvar.wait(lock, wait_pred);
        }
        call_cb(cb, true);
    }

    template <typename Cb>
    void wait_for(timer::duration d, Cb&& cb) {
        bool notified;
        {
            std::unique_lock lock(m_mutex);
            notified = m_cvar.wait_for(lock, d, wait_pred);
        }
        call_cb(cb, notified);
    }

    template <typename Cb>
    void wait_until(timer::time_point t, Cb&& cb) {
        bool notified;
        {
            std::unique_lock lock(m_mutex);
            notified = m_cvar.wait_until(lock, t, wait_pred);
        }
        call_cb(cb, notified);
    }

    template <typename Cb>
    void wait(astl::timeout t, Cb&& cb) {
        if (t.is_infinite()) {
            wait(std::forward<Cb>(cb));
        }
        else {
            wait_for(t.duration, std::forward<Cb>(cb));
        }
    }
};

} // namespace ac::xec
