// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "throw_ex.hpp"
#include <stdexcept>

namespace ac {
using throw_ex = astl::throw_ex<std::runtime_error>;
}
