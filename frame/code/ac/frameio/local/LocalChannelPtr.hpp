// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <memory>

namespace ac::frameio {
class LocalChannel;
using LocalChannelPtr = std::unique_ptr<LocalChannel>;
} // namespace ac::frameio
