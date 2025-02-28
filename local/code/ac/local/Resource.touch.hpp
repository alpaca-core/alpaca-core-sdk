// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"

namespace ac::local {
struct Resource;
namespace impl {
AC_LOCAL_EXPORT void Resource_touch(Resource& resource);
} // namespace impl
} // namespace ac::local
