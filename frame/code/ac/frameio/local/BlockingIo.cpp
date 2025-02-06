// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "BlockingIo.hpp"
#include "../StreamEndpoint.hpp"
#include <future>

namespace ac::frameio {

namespace {

bool waitFor(std::future<void>& f, astl::timeout timeout) {
    if (timeout.is_infinite()) {
        f.wait();
        return true;
    }
    else {
        return std::future_status::timeout != f.wait_for(timeout.duration);
    }
}

struct BlockingFrameIo {
    StreamPtr m_stream;

    BlockingFrameIo(StreamPtr stream) : m_stream(std::move(stream)) {}


    Status io(Frame& frame, astl::timeout timeout) {
        if (timeout.is_zero()) {
            return m_stream->stream(frame, nullptr);
        }

        std::promise<void> promise;
        auto f = promise.get_future();

        {
            auto status = m_stream->stream(frame, [&promise]() {
                return [&promise]() {
                    promise.set_value();
                };
            });

            if (status.complete()) {
                return status;
            }
        }

        Status status;
        if (waitFor(f, timeout)) {
            status.setAborted();
        }
        else {
            status.setTimeout();
        }

        status |= m_stream->stream(frame, nullptr);
        return status;
    }

    void close() {
        m_stream->close();
    }
};

} // namespace

class BlockingIo::Impl {
public:
    BlockingFrameIo m_in;
    BlockingFrameIo m_out;
    //

    bool m_connected = true;

    Impl(StreamEndpoint ep)
        : m_in(std::move(ep.readStream))
        , m_out(std::move(ep.writeStream))
    {}

    ~Impl() {
        close();
    }

    Status poll(Frame& frame, astl::timeout timeout) {
        return m_in.io(frame, timeout);
    }

    Status push(Frame& frame, astl::timeout timeout) {
        return m_out.io(frame, timeout);
    }

    void close() {
        m_in.close();
        m_out.close();
    }
};

BlockingIo::BlockingIo(StreamEndpoint ep)
    : m_impl(std::make_unique<Impl>(std::move(ep)))
{}

BlockingIo::BlockingIo(BlockingIo&&) noexcept = default;
BlockingIo& BlockingIo::operator=(BlockingIo&&) noexcept = default;

BlockingIo::~BlockingIo() = default;

FrameWithStatus BlockingIo::poll(astl::timeout timeout) {
    FrameWithStatus ret;
    ret.status() = m_impl->poll(ret.frame, timeout);
    return ret;
}

Status BlockingIo::poll(Frame& frame, astl::timeout timeout) {
    return m_impl->poll(frame, timeout);
}

Status BlockingIo::push(Frame&& frame, astl::timeout timeout) {
    return m_impl->push(frame, timeout);
}

Status BlockingIo::push(Frame& frame, astl::timeout timeout) {
    return m_impl->push(frame, timeout);
}

void BlockingIo::close() {
    m_impl->close();
}

} // namespace ac::frameio
