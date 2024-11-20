// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "version.h"
#include <astl/version.hpp>
#include <compare>

namespace ac::local {

inline constexpr astl::version Project_Version{
    AC_LOCAL_VERSION_MAJOR,
    AC_LOCAL_VERSION_MINOR,
    AC_LOCAL_VERSION_PATCH
};

} // namespace ac::local
