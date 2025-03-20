// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "timer.hpp"
#include "post.hpp"
#include "wait_func.hpp"
#include "coro_wobj.hpp"

namespace ac::xec {

class timer_wobj {
    strand m_strand;
    timer_ptr m_timer;
public:
    explicit timer_wobj(const strand& s) : m_strand(s), m_timer(timer::create(s)) {}

    void notify_all() {
        post(m_strand, [this] {
            m_timer->cancel();
        });
    }

    void notify_one() {
        post(m_strand, [this] {
            m_timer->cancel_one();
        });
    }

    void wait(wait_func cb) {
        m_timer->expire_never();
        m_timer->add_wait_cb(std::move(cb));
    }

    void wait_for(timer::duration d, wait_func cb) {
        m_timer->expire_after(d);
        m_timer->add_wait_cb(std::move(cb));
    }

    void wait_until(timer::time_point t, wait_func cb) {
        m_timer->expire_at(t);
        m_timer->add_wait_cb(std::move(cb));
    }

    void wait(astl::timeout t, wait_func cb) {
        m_timer->set_timeout(t);
        m_timer->add_wait_cb(std::move(cb));
    }

    void wait_attach(wait_func cb) {
        m_timer->add_wait_cb(std::move(cb));
    }

    using executor_type = strand;
    [[nodiscard]] const strand& get_executor() const {
        return m_strand;
    }

    // corouitne interface implemented in coro_wobj.hpp
    [[nodiscard]] wait_awaitable<timer_wobj> wait() {
        return wait_awaitable(*this);
    }
    [[nodiscard]] timeout_awaitable<timer_wobj> wait(astl::timeout to) {
        return timeout_awaitable(*this, to);
    }
};

} // namespace ac::xec
