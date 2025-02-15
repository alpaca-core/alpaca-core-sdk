// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "status.hpp"
#include <utility>

namespace ac::io {

template <typename StatusLike>
struct status_builder {
    StatusLike object;

    // intentionally not explicit
    template <typename ...Args>
    status_builder(Args&&... args)
        : object(std::forward<Args>(args)...)
    {}

    status_builder(const status_builder&) = delete;
    status_builder& operator=(const status_builder&) = delete;

    status_builder(status_builder&&) noexcept = default;
    status_builder& operator=(status_builder&&) noexcept = default;

    status_builder&& set_success() && noexcept { object.set_success(); return std::move(*this); }
    status_builder&& set_closed()  && noexcept { object.set_closed();  return std::move(*this); }
    status_builder&& set_aborted() && noexcept { object.set_aborted(); return std::move(*this); }
    status_builder&& set_waiting() && noexcept { object.set_waiting(); return std::move(*this); }
    status_builder&& set_timeout() && noexcept { object.set_timeout(); return std::move(*this); }

    operator StatusLike() && { return std::move(object); }
};

} // namespace ac::io
