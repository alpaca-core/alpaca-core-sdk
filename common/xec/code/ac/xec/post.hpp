// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include "task.hpp"
#include "strand.hpp"

namespace ac::xec {

AC_XEC_API void post(const strand& s, task f);

class context;
AC_XEC_API void post(context& ctx, task f);

} // namespace ac::xec
