// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Session.hpp"

// export vtables for classes which only have that

namespace ac {
Session::~Session() = default;
SessionExecutor::~SessionExecutor() = default;
} // namespace ac
