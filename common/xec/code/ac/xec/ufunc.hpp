// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <astl/ufunction.hpp>

namespace ac::xec {
// C++23: replace with std::move_only_function
template <typename F>
using ufunc = astl::ufunction<F>;
} // namespace ac::xec
