// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <system_error>
#include <concepts>

namespace ac::xec {
template <typename T>
concept wait_func_class = std::invocable<T, const std::error_code&>;
} // namespace ac::xec
