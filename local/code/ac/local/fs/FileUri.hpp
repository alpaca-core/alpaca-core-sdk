// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../export.h"
#include <string>
#include <string_view>

namespace ac::local::fs {

AC_LOCAL_EXPORT std::string FileUri_toPath(std::string_view uri);
AC_LOCAL_EXPORT std::string FileUri_fromPath(std::string_view path);

} // namespace ac::local::fs
