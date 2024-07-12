// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <memory>

namespace astl {

template <typename T>
using c_unique_ptr = std::unique_ptr<T, void(*)(T*)>;

} // namespace astl
