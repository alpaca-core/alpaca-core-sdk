// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Io.hpp"
#include "IoExecutor.hpp"

namespace ac::frameio {
// just export vtable
Input::~Input() = default;
Output::~Output() = default;
IoExecutor::~IoExecutor() = default;
} // namespace ac::frameio
