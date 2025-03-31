// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../export.h"
#include <string>
#include <cstdint>
#include <string_view>

namespace ac::local::fs {

AC_LOCAL_EXPORT std::string getTempDir();

AC_LOCAL_EXPORT std::string getFileName(std::string_view path);

AC_LOCAL_EXPORT void expandPathInPlace(std::string& path); // expands in place
AC_LOCAL_EXPORT std::string expandPath(std::string_view path);

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

AC_LOCAL_EXPORT BasicStat basicStat(const std::string& path) noexcept;

AC_LOCAL_EXPORT bool mkdir_p(std::string_view path);
AC_LOCAL_EXPORT bool rm_r(std::string_view path, bool f = false);
AC_LOCAL_EXPORT void touch(const std::string& path, bool mkdir = true);

AC_LOCAL_EXPORT void mv(std::string_view src, std::string_view tgt);

} // namespace ac::local::fs
