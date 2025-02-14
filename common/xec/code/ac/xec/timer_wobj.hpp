// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "timer.hpp"
#include "post.hpp"

namespace ac::xec {

class timer_wobj {
    strand m_strand;
    timer_ptr m_timer;
public:
    timer_wobj(strand& s) : m_strand(s), m_timer(timer::create(s)) {}

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
};

} // namespace ac::xec
