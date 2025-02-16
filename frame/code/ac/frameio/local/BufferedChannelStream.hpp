// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "BufferedChannel.hpp"
#include "ChannelEndpoints.hpp"
#include "../StreamEndpoint.hpp"
#include "../StreamPtr.hpp"
#include <ac/io/channel_stream.hpp>

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

    virtual io::stream_result read(Frame& f, xec::notifiable* nobj) override {
        return m_stream.read(f, nobj);
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

    virtual io::stream_result write(Frame& f, xec::notifiable* nobj) override {
        return m_stream.write(f, nobj);
    }

    virtual void close() override {
        m_stream.close();
    }
};

inline ChannelEndpoints BufferedChannel_getEndpoints(std::unique_ptr<BufferedChannel> a, std::unique_ptr<BufferedChannel> b) {
    auto as = io::make_channel_streams<BufferedChannel, BufferedChannelReadStream, BufferedChannelWriteStream>(std::move(a));
    auto bs = io::make_channel_streams<BufferedChannel, BufferedChannelReadStream, BufferedChannelWriteStream>(std::move(b));

    ChannelEndpoints ret;
    ret.ab.read_stream = std::move(as.out);
    ret.ab.write_stream = std::move(bs.in);
    ret.ba.read_stream = std::move(bs.out);
    ret.ba.write_stream = std::move(as.in);
    return ret;
}

} // namespace ac::frameio
