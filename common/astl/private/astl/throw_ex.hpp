// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <itlib/throw_ex.hpp>
#include <stdexcept>

namespace ac {
using throw_ex = itlib::throw_ex<std::runtime_error>;
}
