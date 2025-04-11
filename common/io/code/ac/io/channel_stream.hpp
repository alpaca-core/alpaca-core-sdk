// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "concepts/channel.hpp"
#include "concepts/stream.hpp"
#include <memory>

namespace ac::io {

// convert function as template arg? (default std::move)
// channel payload as template arg? (default shared_ptr)
template <typename Channel>
class channel_read_stream {
    static_assert(channel_class<Channel>);
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
    template <on_blocked_cb_class OnBlocked>
    stream_result read(read_value_type& value, OnBlocked&& on_blocked) {
        return m_channel->read(value, std::forward<OnBlocked>(on_blocked));
    }
    void close() {
        m_channel->close();
    }
};

template <typename Channel>
class channel_write_stream {
    static_assert(channel_class<Channel>);
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
    template <on_blocked_cb_class OnBlocked>
    stream_result write(write_value_type& value, OnBlocked&& on_blocked) {
        return m_channel->write(value, std::forward<OnBlocked>(on_blocked));
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
