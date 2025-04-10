// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Stream.hpp"
#include <ac/io/blocking_io.hpp>

namespace ac::frameio {
using BlockingIo = io::blocking_io<ReadStream, WriteStream>;
} // namespace ac::frameio
