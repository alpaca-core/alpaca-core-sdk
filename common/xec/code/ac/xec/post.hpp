// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include "ufunc.hpp"
#include "strand.hpp"

namespace ac::xec {

AC_XEC_API void post(const strand& s, ufunc<void()> f);

class context;
AC_XEC_API void post(context& ctx, ufunc<void()> f);

} // namespace ac::xec
