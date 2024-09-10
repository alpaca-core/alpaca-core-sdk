// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "FsUtil.hpp"
#include <astl/throw_ex.hpp>
#include <cstdlib>
#include <sys/stat.h>
#if defined(_WIN32)
#   define stat _stat
#endif

namespace ac::fs {

constexpr const char* Home_Var =
#if defined(_WIN32)
    "USERPROFILE";
#else
    // just use $HOME for everything else
    "HOME";
#endif
;

void expandPathInPlace(std::string& path) {
    if (path.starts_with('~')) {
        auto home = std::getenv(Home_Var);

        if (!home) {
            throw_ex{} << "Environment variable " << Home_Var << " not set";
        }

        path.replace(0, 1, home);
        return;
    }

    if (!path.starts_with('$')) return;

    auto end = path.find_first_of('/');
    if (end == std::string::npos) {
        end = path.size();
    }

    auto var = path.substr(1, end - 1);
    auto val = std::getenv(var.data());
    if (!val) {
        throw_ex{} << "Environment variable " << var << " not set";
    }

    path.replace(0, end, val);
}


std::string expandPath(std::string_view path) {
    auto ret = std::string(path);
    expandPathInPlace(ret);
    return ret;
}

BasicStat basicStat(const std::string& path) noexcept {
    struct stat st;
    if (stat(path.data(), &st) != 0) {
        return {BasicStat::NotExist, 0};
    }
    if (st.st_mode & S_IFREG) {
        return {BasicStat::File, static_cast<uint64_t>(st.st_size)};
    }
    if (st.st_mode & S_IFDIR) {
        return {BasicStat::Directory, 0};
    }
    return {BasicStat::Other, 0};
}

} // namespace ac::fs
