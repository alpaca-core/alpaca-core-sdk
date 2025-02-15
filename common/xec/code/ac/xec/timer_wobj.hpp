// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "notifiable.hpp"
#include "timer.hpp"
#include "post.hpp"
#include "wait_func.hpp"

namespace ac::xec {

class timer_wobj final : public notifiable {
    strand m_strand;
    timer_ptr m_timer;
public:
    explicit timer_wobj(const strand& s) : m_strand(s), m_timer(timer::create(s)) {}

    void notify_all() override {
        post(m_strand, [this] {
            m_timer->cancel();
        });
    }

    void notify_one() override {
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

    using executor_type = strand;
    const strand& get_executor() const {
        return m_strand;
    }
};

} // namespace ac::xec
