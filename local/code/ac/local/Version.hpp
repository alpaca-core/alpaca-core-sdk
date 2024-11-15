// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "version.h"
#include <compare>

namespace ac::local {
struct Version {
    int major;
    int minor;
    int patch;

    constexpr Version(int major, int minor, int patch)
        : major(major), minor(minor), patch(patch)
    {}

    auto operator<=>(const Version&) const = default;
};

inline constexpr Version Project_Version(
    AC_LOCAL_VERSION_MAJOR,
    AC_LOCAL_VERSION_MINOR,
    AC_LOCAL_VERSION_PATCH
);

} // namespace ac::local
