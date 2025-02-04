// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "IoException.hpp"

namespace ac::frameio {
// export the vtable
IoException::~IoException() = default;
IoClosed::~IoClosed() = default;
} // namespace ac::frameio
