// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "stream.hpp"

namespace ac::io {

template <typename T>
concept channel_class = read_class<T> && write_class<T>;

} // namespace ac::io