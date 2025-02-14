// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include "strand.hpp"
#include "wait_func.hpp"
#include "timer_ptr.hpp"
#include <chrono>
#include <cstddef>

namespace ac::xec {

class AC_XEC_API timer {
public:
    virtual ~timer();

    using clock_type = std::chrono::steady_clock;
    using duration = clock_type::duration;
    using time_point = clock_type::time_point;

    timer(const timer&) = delete;
    timer& operator=(const timer&) = delete;

    virtual size_t expire_after(duration t_from_now) = 0;
    virtual size_t expire_at(time_point t) = 0;
    virtual size_t expire_never() = 0;

    virtual size_t cancel() = 0;
    virtual size_t cancel_one() = 0;

    virtual time_point expiry() const = 0;

    virtual void add_wait_cb(wait_func cb) = 0;

    static timer_ptr create(const strand& s);
private:
    // sealed interface
    timer();
    friend struct timer_impl;
};

} // namespace ac::xec
