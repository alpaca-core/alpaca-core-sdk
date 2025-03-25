// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "SysFrameOpPrefix.h"
#include <string>

namespace ac::schema {

inline namespace sys {

struct Error {
    static constexpr auto id = AC_SYSFRAME_OP_PREFIX "err";

    using Type = std::string;
};

} // namespace sys

} // namespace ac::schema
