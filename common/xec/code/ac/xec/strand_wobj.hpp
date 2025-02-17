// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "notifiable.hpp"
#include "post.hpp"
#include "wait_func.hpp"
#include "wait_func_invoke.hpp"
#include "coro_wobj.hpp"

namespace ac::xec {

class strand_wobj final : public notifiable {
    strand m_strand;
    wait_func m_cb;
public:
    explicit strand_wobj(const strand& s) : m_strand(s) {}

    void notify_all() override {
        notify_one();
    }

    void notify_one() override {
        post(m_strand, [this] {
            if (m_cb) {
                auto cb = std::exchange(m_cb, nullptr);
                wait_func_invoke_cancelled(cb);
            }
        });
    }

    void wait(wait_func cb) {
        if (m_cb) {
            post(m_strand, [old = std::move(m_cb)] {
                wait_func_invoke_cancelled(old);
            });
        }
        m_cb = std::move(cb);
    }

    using executor_type = strand;
    const strand& get_executor() const {
        return m_strand;
    }

    // corouitne interface implemented in coro_wobj.hpp
    [[nodiscard]] wait_awaitable<strand_wobj> wait() {
        return wait_awaitable(*this);
    }
};

} // namespace ac::xec
