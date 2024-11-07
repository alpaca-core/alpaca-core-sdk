// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#pragma once
#include "version.h"
#include <string_view>

namespace ac::local {
struct Version {
    int major;
    int minor;
    int patch;
    std::string_view tag;

    constexpr Version(int major, int minor, int patch, std::string_view tag)
        : major(major), minor(minor), patch(patch), tag(tag)
    {}

    auto operator<=>(const Version&) const = default;
};

inline constexpr Version Project_Version(
    AC_VERSION_MAJOR,
    AC_VERSION_MINOR,
    AC_VERSION_PATCH,
    AC_VERSION_TAG
);

} // namespace ac::local
