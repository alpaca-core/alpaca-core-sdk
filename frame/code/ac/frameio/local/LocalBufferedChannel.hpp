// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../../export.h"
#include "LocalChannelPtr.hpp"

namespace ac::frameio {
AC_FRAME_EXPORT LocalChannelPtr LocalBufferedChannel_create(size_t size);
} // namespace ac::frameio
