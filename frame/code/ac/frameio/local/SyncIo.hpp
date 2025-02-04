// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../../export.h"
#include "../StreamEndpoint.hpp"
#include "../SessionHandlerPtr.hpp"

namespace ac::frameio {
// create a session which exports its executor to the caller
[[nodiscard]] AC_FRAME_EXPORT std::function<void()> Session_connectSync(SessionHandlerPtr handler, StreamEndpoint endpoint);
} // namespace ac::frameio
