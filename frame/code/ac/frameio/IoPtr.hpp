// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Io.hpp"
#include <memory>

namespace ac::frameio {
using InputPtr = std::unique_ptr<Input>;
using OutputPtr = std::unique_ptr<Output>;
} // namespace ac::frameio
