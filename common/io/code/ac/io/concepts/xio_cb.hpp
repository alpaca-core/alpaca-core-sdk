// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <concepts>

namespace ac::io {
struct status;

template <typename T, typename V>
concept xio_cb_class = std::invocable<T, V&, const status&>;
} // namespace ac::io
