// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Stream.hpp"
#include <ac/io/stream_endpoint.hpp>

namespace ac::frameio {
using StreamEndpoint = ac::io::stream_endpoint<ReadStream, WriteStream>;
} // namespace ac::frameio
