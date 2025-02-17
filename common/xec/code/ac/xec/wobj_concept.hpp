// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "notifiable.hpp"
#include <astl/timeout.hpp>
#include <system_error>
#include <concepts>

namespace ac::xec {
template <typename T>
concept basic_wait_object_class =
std::derived_from<T, notifiable>
&& requires(T wobj) {
    typename T::executor_type;
    { wobj.get_executor() } -> std::convertible_to<typename T::executor_type>;
};

template <typename T>
concept wait_object_class =
basic_wait_object_class<T>
&& requires(T wobj, void(*cb)(const std::error_code&)) {
    wobj.wait(cb);
};

template <typename T>
concept timeout_wait_object_class =
basic_wait_object_class<T>
&& requires(T wobj, astl::timeout to, void(*cb)(const std::error_code&)) {
    wobj.wait(to, cb);
};
} // namespace xec
