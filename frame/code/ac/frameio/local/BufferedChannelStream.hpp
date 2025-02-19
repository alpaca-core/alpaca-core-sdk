// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "BufferedChannel.hpp"
#include "ChannelEndpoints.hpp"
#include "../StreamEndpoint.hpp"
#include "../StreamPtr.hpp"
#include <ac/io/buffered_channel_endpoints.hpp>

namespace ac::frameio {

struct ChannelStreams {
    ReadStreamPtr in;
    WriteStreamPtr out;
};

class BufferedChannelReadStream final : public ReadStream {
public:
    io::channel_read_stream<BufferedChannel> m_stream;

    BufferedChannelReadStream(std::shared_ptr<BufferedChannel> channel)
        : m_stream(channel)
    {}

    virtual io::stream_result read(Frame& f, OnBlockedFunc onBlocked) override {
        return m_stream.read(f, std::move(onBlocked));
    }

    virtual void close() override {
        m_stream.close();
    }
};

class BufferedChannelWriteStream final : public WriteStream {
public:
    io::channel_write_stream<BufferedChannel> m_stream;

    BufferedChannelWriteStream(std::shared_ptr<BufferedChannel> channel)
        : m_stream(channel)
    {}

    virtual io::stream_result write(Frame& f, OnBlockedFunc onBlocked) override {
        return m_stream.write(f, std::move(onBlocked));
    }

    virtual void close() override {
        m_stream.close();
    }
};

inline ChannelEndpoints BufferedChannel_getEndpoints(BufferedChannelPtr a, BufferedChannelPtr b) {
    return make_channel_endpoints<Frame, BufferedChannelReadStream, BufferedChannelWriteStream>(
        std::move(a),
        std::move(b)
    );
}

inline ChannelEndpoints BufferedChannel_getEndpoints(size_t sa, size_t sb) {
    return ac::io::make_buffered_channel_endpoints<Frame, BufferedChannelReadStream, BufferedChannelWriteStream>(
        sa,
        sb
    );
}

} // namespace ac::frameio
