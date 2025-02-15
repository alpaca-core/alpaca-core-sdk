// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "ufunc.hpp"
#include "wait_func_invoke.hpp"
#include <system_error>

namespace ac::xec {
using wait_func = ufunc<void(const std::error_code& cancelled)>;
} // namespace ac::xec
