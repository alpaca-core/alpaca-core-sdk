// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <memory>

namespace ac::frameio {
class Input;
using InputPtr = std::unique_ptr<Input>;
class Output;
using OutputPtr = std::unique_ptr<Output>;
class IoExecutor;
using IoExecutorPtr = std::unique_ptr<IoExecutor>;
} // namespace ac::frameio
