// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "exception.hpp"

namespace ac::io {
// export the vtables
exception::~exception() = default;
stream_closed_error::~stream_closed_error() = default;
} // namespace ac::io
