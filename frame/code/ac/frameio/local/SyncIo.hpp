// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../../export.h"
#include "../StreamEndpoint.hpp"
#include "../SessionHandlerPtr.hpp"

namespace ac::frameio {
// createa a synchronous session which "hijacks" the thread of its peer for its execution
// since the only notification from the streams we get is on when they are free for more queries,
// this can only work with buffers of size exactly 1
AC_FRAME_EXPORT void Session_connectSync(SessionHandlerPtr handler, StreamEndpoint endpoint);
} // namespace ac::frameio
