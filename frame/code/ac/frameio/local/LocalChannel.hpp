// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../Stream.hpp"

namespace ac::frameio {

class AC_FRAME_EXPORT LocalChannel {
public:
    virtual ~LocalChannel();
    virtual io::status read(Frame& f, Stream::OnBlockedFunc onBlocked) = 0;
    virtual io::status write(Frame& f, Stream::OnBlockedFunc onBlocked) = 0;
    virtual void close() = 0;
};

using LocalChannelPtr = std::unique_ptr<LocalChannel>;

namespace impl {
using LocalChannelSharedPtr = std::shared_ptr<LocalChannel>;
class LocalStream {
protected:
    LocalChannelSharedPtr m_channel;
    ~LocalStream() = default;
public:
    LocalStream() = default;

    // intentionally implicit
    LocalStream(LocalChannelSharedPtr channel) : m_channel(std::move(channel)) {}

    explicit operator bool() const { return !!m_channel; }
};
}

class LocalReadStream final : public impl::LocalStream, public ReadStream {
public:
    using impl::LocalStream::LocalStream;
    ~LocalReadStream() {
        close();
    }
    io::status read(Frame& f, OnBlockedFunc onBlocked) override {
        return m_channel->read(f, std::move(onBlocked));
    }
    void close() override {
        m_channel->close();
    }
};

class LocalWriteStream final : public impl::LocalStream, public WriteStream {
public:
    using impl::LocalStream::LocalStream;
    ~LocalWriteStream() {
        close();
    }
    io::status write(Frame& f, OnBlockedFunc onBlocked) override {
        return m_channel->write(f, std::move(onBlocked));
    }
    void close() override {
        m_channel->close();
    }
};

using LocalReadStreamPtr = std::unique_ptr<LocalReadStream>;
using LocalWriteStreamPtr = std::unique_ptr<LocalWriteStream>;

struct LocalChannelStreams {
    LocalReadStreamPtr in;
    LocalWriteStreamPtr out;
};

inline LocalChannelStreams LocalChannel_getStreams(LocalChannelPtr channel) {
    impl::LocalChannelSharedPtr sharedChannel = std::move(channel);
    LocalChannelStreams streams;
    streams.in = std::make_unique<LocalReadStream>(sharedChannel);
    streams.out = std::make_unique<LocalWriteStream>(sharedChannel);
    return streams;
}

} // namespace ac::frameio
