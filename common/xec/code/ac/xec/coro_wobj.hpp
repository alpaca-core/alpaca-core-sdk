// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <astl/timeout.hpp>
#include <coroutine>
#include <system_error>

namespace ac::xec {

struct basic_wait_awaitable {
    bool ret = false;
    bool await_ready() const noexcept { return false; }
    bool await_resume() noexcept { return ret; }
};

template <typename Wobj>
struct wait_awaitable : basic_wait_awaitable {
    Wobj& wobj;
    wait_awaitable(Wobj& w) : wobj(w) {}
    void await_suspend(std::coroutine_handle<> h) {
        wobj.wait([this, h](const std::error_code& ec) {
            ret = !!ec;
            h.resume();
        });
    }
};

template <typename Wobj>
struct timeout_awaitable : basic_wait_awaitable {
    Wobj& wobj;
    astl::timeout to;
    timeout_awaitable(Wobj& w, astl::timeout t) : wobj(w), to(t) {}
    void await_suspend(std::coroutine_handle<> h) {
        wobj.wait(to, [this, h](const std::error_code& ec) {
            ret = !!ec;
            h.resume();
        });
    }
};

} // namespace ac::xec
