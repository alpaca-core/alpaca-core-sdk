// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../Frame.hpp"
#include <ac/io/value_with_status.hpp>

namespace ac::frameio {
using FrameWithStatus = io::value_with_status<Frame>;
} // namespace ac::frameio
