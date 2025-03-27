// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "SysFrameOpPrefix.h"
#include <cstddef>

namespace ac::schema {

inline namespace sys {

struct Abort {
    static constexpr auto id = AC_SYSFRAME_OP_PREFIX "abort";
    using Type = std::nullptr_t;
};

} // namespace sys

} // namespace ac::schema
