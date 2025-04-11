// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

namespace ac::io {
template <typename RS, typename WS>
struct stream_endpoint;
} // namespace ac::io

namespace ac::frameio {
class ReadStream;
class WriteStream;
using StreamEndpoint = ac::io::stream_endpoint<ReadStream, WriteStream>;
} // namespace ac::frameio
