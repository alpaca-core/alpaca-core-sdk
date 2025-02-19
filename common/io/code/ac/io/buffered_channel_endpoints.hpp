// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "buffered_channel.hpp"
#include "channel_stream.hpp"
#include "channel_endpoints.hpp"

namespace ac::io {

template <
    typename T,
    read_stream_class ReadStream = channel_read_stream<buffered_channel<T>>,
    write_stream_class WriteStream = channel_write_stream<buffered_channel<T>>
>
channel_streams<ReadStream, WriteStream> make_buffered_channel_streams(std::unique_ptr<buffered_channel<T>> uchannel) {
    std::shared_ptr<buffered_channel<T>> channel(std::move(uchannel));
    return {
        std::make_unique<ReadStream>(channel),
        std::make_unique<WriteStream>(channel)
    };
}

template <
    typename T,
    read_stream_class ReadStream = channel_read_stream<buffered_channel<T>>,
    write_stream_class WriteStream = channel_write_stream<buffered_channel<T>>
>
auto make_channel_endpoints(std::unique_ptr<buffered_channel<T>> a, std::unique_ptr<buffered_channel<T>> b) {
    return make_channel_endpoints(
        make_buffered_channel_streams<T, ReadStream, WriteStream>(std::move(a)),
        make_buffered_channel_streams<T, ReadStream, WriteStream>(std::move(b))
    );
}

template <
    typename T,
    read_stream_class ReadStream = channel_read_stream<buffered_channel<T>>,
    write_stream_class WriteStream = channel_write_stream<buffered_channel<T>>
>
auto make_buffered_channel_endpoints(size_t sa, size_t sb) {
    return make_channel_endpoints(
        make_buffered_channel_streams<T, ReadStream, WriteStream>(std::make_unique<buffered_channel<T>>(sa)),
        make_buffered_channel_streams<T, ReadStream, WriteStream>(std::make_unique<buffered_channel<T>>(sb))
    );
}

} // namespace ac::io
