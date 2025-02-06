// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../Stream.hpp"
#include "LocalChannelPtr.hpp"

namespace ac::frameio {

class AC_FRAME_EXPORT LocalChannel {
public:
    virtual ~LocalChannel();
    virtual Status read(Frame& f, Stream::OnBlockedFunc onBlocked) = 0;
    virtual void cancelReadBlock() = 0;
    virtual Status write(Frame& f, Stream::OnBlockedFunc onBlocked) = 0;
    virtual void cancelWriteBlock() = 0;
    virtual void close() = 0;
    virtual bool closed() = 0;
};

namespace impl {
class LocalStream {
protected:
    LocalChannelPtr m_channel;
    ~LocalStream() = default;
public:
    LocalStream() = default;

    // intentionally implicit
    LocalStream(LocalChannelPtr channel) : m_channel(std::move(channel)) {}

    explicit operator bool() const { return !!m_channel; }
};
}

class LocalReadStream final : public impl::LocalStream, public ReadStream {
public:
    using impl::LocalStream::LocalStream;
    Status read(Frame& f, OnBlockedFunc onBlocked) override {
        return m_channel->read(f, std::move(onBlocked));
    }
    void close() override {
        m_channel->close();
    }
};

class LocalWriteStream final : public impl::LocalStream, public WriteStream {
public:
    using impl::LocalStream::LocalStream;
    Status write(Frame& f, OnBlockedFunc onBlocked) override {
        return m_channel->write(f, std::move(onBlocked));
    }
    void close() override {
        m_channel->close();
    }
};

} // namespace ac::frameio
