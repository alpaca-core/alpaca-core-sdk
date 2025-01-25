// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../../export.h"
#include "../StreamEndpoint.hpp"
#include "../SessionHandlerPtr.hpp"

namespace ac::frameio {
// create a synchronous session which can use the thread of its peer for its execution
// return a function that will execute the pending taasks of the session executor
[[nodiscard]] AC_FRAME_EXPORT std::function<void()> Session_connectSync(SessionHandlerPtr handler, StreamEndpoint endpoint);
} // namespace ac::frameio
