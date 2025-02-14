// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include <memory>

namespace ac::xec {
struct strand_impl; // opaque
using strand = std::shared_ptr<strand_impl>;

class context;
AC_XEC_API strand make_strand(context& ctx);
} // namespace ac::xec
