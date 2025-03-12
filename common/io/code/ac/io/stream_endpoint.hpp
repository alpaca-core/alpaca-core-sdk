// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "concepts/stream.hpp"
#include <memory>

namespace ac::io {

template <typename RS, typename WS>
struct stream_endpoint {
    static_assert(read_stream_class<RS>);
    static_assert(write_stream_class<WS>);

    std::unique_ptr<RS> read_stream;
    std::unique_ptr<WS> write_stream;
};

} // namespace ac::io
