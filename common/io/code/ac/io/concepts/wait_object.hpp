// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/xec/notifiable.hpp>
#include <astl/timeout.hpp>
#include <system_error>
#include <concepts>

namespace ac::io {
template <typename T>
concept wait_object_class =
    std::derived_from<T, xec::notifiable>
    && requires(T wobj) {
        typename T::executor_type;
        { wobj.get_executor() } -> std::convertible_to<typename T::executor_type>;
    }
    && requires(T wobj, void(*cb)(const std::error_code&)) {
        wobj.wait(std::declval<astl::timeout>(), cb);
    };
} // namespace ac::io
