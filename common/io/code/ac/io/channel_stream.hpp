// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "concepts/channel.hpp"
#include <memory>

namespace ac::io {

// convert function as template arg? (default std::move)
// channel payload as template arg? (default shared_ptr)
template <channel_class Channel>
class channel_read_stream {
    std::shared_ptr<Channel> m_channel;
public:
    using channel_type = Channel;
    using read_value_type = typename channel_type::read_value_type;
    using value_type = read_value_type;

    channel_read_stream(std::shared_ptr<Channel> channel)
        : m_channel(std::move(channel))
    {}
    ~channel_read_stream() {
        close();
    }
    stream_result read(read_value_type& value, xec::notifiable* notify) {
        return m_channel->read(value, notify);
    }
    void close() {
        m_channel->close();
    }
};

template <channel_class Channel>
class channel_write_stream {
    std::shared_ptr<Channel> m_channel;
public:
    using channel_type = Channel;
    using write_value_type = typename channel_type::write_value_type;
    using value_type = write_value_type;

    channel_write_stream(std::shared_ptr<Channel> channel)
        : m_channel(std::move(channel))
    {}
    ~channel_write_stream() {
        close();
    }
    stream_result write(write_value_type& value, xec::notifiable* notify) {
        return m_channel->write(value, notify);
    }
    void close() {
        m_channel->close();
    }
};

template <read_stream_class ReadStream, write_stream_class WriteStream>
struct channel_streams {
    std::unique_ptr<ReadStream> out;
    std::unique_ptr<WriteStream> in;
};

template <
    channel_class Channel,
    read_stream_class ReadStream = channel_read_stream<Channel>,
    write_stream_class WriteStream = channel_write_stream<Channel>
>
channel_streams<ReadStream, WriteStream> make_channel_streams(std::unique_ptr<Channel> uchannel) {
    std::shared_ptr<Channel> channel(std::move(uchannel));
    return {
        std::make_unique<ReadStream>(channel),
        std::make_unique<WriteStream>(channel)
    };
}

} // namespace ac::io
