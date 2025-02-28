// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <chrono>

namespace ac::local {
struct Resource;
using time_point_t = std::chrono::steady_clock::time_point;
namespace impl {
AC_LOCAL_EXPORT void Resource_touch(Resource& resource);

AC_LOCAL_EXPORT time_point_t Resource_getExpireTime(const Resource& resource);
} // namespace impl
} // namespace ac::local
