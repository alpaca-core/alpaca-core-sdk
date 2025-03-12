// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/io/stream_endpoint_fwd.hpp>

namespace ac::frameio {
class ReadStream;
class WriteStream;
using StreamEndpoint = ac::io::stream_endpoint<ReadStream, WriteStream>;
} // namespace ac::frameio
