// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Provider.hpp"
#include <memory>

namespace ac::local {
using ProviderPtr = std::unique_ptr<Provider>;
} // namespace ac::local
