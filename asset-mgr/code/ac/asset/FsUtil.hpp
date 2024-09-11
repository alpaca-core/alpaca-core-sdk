// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <string>
#include <cstdint>
#include <string_view>

namespace ac::asset::fs {

AC_ASSET_EXPORT void expandPathInPlace(std::string& path); // expands in place
AC_ASSET_EXPORT std::string expandPath(std::string_view path);

struct BasicStat {
    enum Type {
        NotExist,
        File,
        Directory,
        Other
    } type;
    uint64_t size;

    bool exists() const noexcept { return type != NotExist; }
    bool file() const noexcept { return type == File; }
    bool dir() const noexcept { return type == Directory; }
    bool other() const noexcept { return type == Other; }
};

AC_ASSET_EXPORT BasicStat basicStat(const std::string& path) noexcept;

} // namespace ac::asset::fs
