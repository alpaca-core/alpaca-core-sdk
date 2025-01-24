// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include <chrono>

namespace astl {

// helper type to represent a timeout

template <typename Rep>
struct t_timeout {
    using rep_t = Rep;
    std::chrono::duration<rep_t, std::milli> duration;

    constexpr t_timeout() = default;
    constexpr t_timeout(std::chrono::milliseconds d) : duration(d) {} // intentionally not explicit

    static constexpr t_timeout after(std::chrono::milliseconds d) { return t_timeout{d}; }
    static constexpr t_timeout after_ms(rep_t ms) { return after(std::chrono::milliseconds(ms)); }
    static constexpr t_timeout never() { return t_timeout{std::chrono::milliseconds(-1)}; }
    static constexpr t_timeout now() { return t_timeout{std::chrono::milliseconds(0)}; }
    static constexpr t_timeout immediately() { return now(); }

    constexpr rep_t ms() const { return duration.count(); };
    constexpr bool infinite() const { return ms() < 0; }
    constexpr bool zero() const { return ms() == 0; }
};

// we don't expect huge timeouts, so no point in wasting bits by default
using timeout = t_timeout<int32_t>;

namespace timeout_vals {
inline constexpr timeout await_completion_for(std::chrono::milliseconds d) { return timeout::after(d); }
inline constexpr timeout await_completion = timeout::never();
inline constexpr timeout no_wait = timeout::now();
inline constexpr timeout proceed_immediately = timeout::now();
} // namespace timeout_ctors

using huge_timeout = t_timeout<int64_t>;

} // namespace astl
