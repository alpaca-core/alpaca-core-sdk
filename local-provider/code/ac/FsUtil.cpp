// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "FsUtil.hpp"

#include <sys/stat.h>
#if defined(_WIN32)
#   define stat _stat
#endif

namespace ac::fs {

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
