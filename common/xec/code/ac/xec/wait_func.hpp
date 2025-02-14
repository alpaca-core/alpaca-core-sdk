// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "ufunc.hpp"
#include <system_error>

namespace ac::xec {
using wait_func = ufunc<void(const std::error_code& cancelled)>;

inline void wait_func_invoke_timeout(wait_func& cb) {
    cb({});
}

inline void wait_func_invoke_cancelled(wait_func& cb) {
    cb(std::make_error_code(std::errc::operation_canceled));
}

} // namespace ac::xec
