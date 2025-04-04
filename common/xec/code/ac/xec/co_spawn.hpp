// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "coro.hpp"
#include "post.hpp"

namespace ac::xec {

inline void co_spawn(strand ex, coro<void> c) {
    auto h = c.take_handle();
    h.promise().m_executor = ex;
    post(ex, [h] {
        h.resume();
    });
}

inline void co_spawn(context& ctx, coro<void> c) {
    co_spawn(make_strand(ctx), std::move(c));
}

} // namespace ac::xec
