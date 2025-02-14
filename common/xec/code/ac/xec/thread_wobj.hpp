// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "wait_func.hpp"
#include <condition_variable>
#include <atomic>

namespace ac::xec {

class thread_wobj {
    alignas(std::hardware_destructive_interference_size)
        std::atomic_bool m_flag;

    std::mutex m_mutex;
    std::condition_variable m_cvar;

    bool wait_pred() {
        return m_flag.exchange(false, std::memory_order_acquire);
    }

    void call_cb(wait_func& cb, bool notified) {
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

    void wait(wait_func cb) {
        {
            std::unique_lock lock(m_mutex);
            m_cvar.wait(lock, wait_pred);
        }
        call_cb(cb, true);
    }

    void wait_for(timer::duration d, wait_func cb) {
        bool notified;
        {
            std::unique_lock lock(m_mutex);
            notified = m_cvar.wait_for(lock, d, wait_pred);
        }
        call_cb(cb, notified);
    }

    void wait_until(timer::time_point t, wait_func cb) {
        bool notified;
        {
            std::unique_lock lock(m_mutex);
            notified = m_cvar.wait_until(lock, t, wait_pred);
        }
        call_cb(cb, notified);
    }

    void wait(astl::timeout t, wait_func cb) {
        if (t.is_infinite()) {
            wait(std::move(cb));
        }
        else {
            wait_for(t.duration, std::move(cb));
        }
    }
};

} // namespace ac::xec
