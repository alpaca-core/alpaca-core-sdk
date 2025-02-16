// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "channel_stream.hpp"
#include "stream_endpoint.hpp"

namespace ac::io {

template <channel_class CA, channel_class CB>
struct channel_endpoints {
    stream_endpoint<channel_read_stream<CA>, channel_write_stream<CB>> ab;
    stream_endpoint<channel_read_stream<CB>, channel_write_stream<CA>> ba;
};

template <channel_class CA, channel_class CB>
channel_endpoints<CA, CB> make_channel_endpoints(std::unique_ptr<CA> a, std::unique_ptr<CB> b) {
    auto as = make_channel_streams(std::move(a));
    auto bs = make_channel_streams(std::move(b));
    return {
        {std::move(as.out), std::move(bs.in)},
        {std::move(bs.out), std::move(as.in)}
    };
}

} // namespace ac::io