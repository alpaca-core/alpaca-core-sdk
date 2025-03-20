// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <chrono>

namespace ac::local {

struct Resource {
    using seconds_t = std::chrono::duration<int32_t>;
    seconds_t maxAge = seconds_t(10);
    using time_point_t = std::chrono::steady_clock::time_point;

    void touch() noexcept {
        m_expireTime = std::chrono::steady_clock::now() + maxAge;
    }

    time_point_t expireTime() const noexcept {
        return m_expireTime;
    }

protected:
    ~Resource() = default;

private:
    time_point_t m_expireTime = time_point_t::min();
};

} // namespace ac::local
