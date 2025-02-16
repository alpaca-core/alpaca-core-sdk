// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "concepts/stream.hpp"
#include <memory>

namespace ac::io {

template <read_stream_class RS, write_stream_class WS>
struct stream_endpoint {
    std::unique_ptr<RS> read_stream;
    std::unique_ptr<WS> write_stream;
};

} // namespace ac::io
