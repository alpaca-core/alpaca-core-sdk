// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "BlockingIo.hpp"
#include "../IoCommon.hpp"
#include "../BasicStreamIo.hpp"
#include "../StreamEndpoint.hpp"
#include <future>

namespace ac::frameio {

namespace {

bool waitFor(std::future<void>& f, astl::timeout timeout) {
    if (timeout.infinite()) {
        f.wait();
        return true;
    }
    else {
        return std::future_status::timeout != f.wait_for(timeout.duration);
    }
}

struct BlockingFrameIo : public BasicStreamIo {
    using BasicStreamIo::BasicStreamIo;

    Status io(Frame& frame) {
        return m_stream->stream(frame, nullptr);
    }

    void io(Frame& frame, astl::timeout timeout, IoCb cb) {
        std::promise<void> promise;
        auto f = promise.get_future();

        {
            auto status = m_stream->stream(frame, [&promise]() {
                return [&promise]() {
                    promise.set_value();
                };
            });

            if (status.complete()) {
                return cb(frame, status);
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
        cb(frame, status);
    }
};

using BlockingFrameInput = InputCommon<BlockingFrameIo>;
using BlockingFrameOutput = OutputCommon<BlockingFrameIo>;

} // namespace

class BlockingIo::Impl {
public:
    BlockingFrameInput m_in;
    BlockingFrameOutput m_out;
    //

    bool m_connected = true;

    Impl(StreamEndpoint ep)
        : m_in(std::move(ep.readStream))
        , m_out(std::move(ep.writeStream))
    {}

    ~Impl() {
        close();
    }

    Status io(BlockingFrameIo& q, Frame& frame, astl::timeout timeout) {
        Status ret;
        q.io(frame, timeout, [&](Frame&, Status status) {
            ret = status;
        });
        return ret;
    }

    Status poll(Frame& frame, astl::timeout timeout) {
        return io(m_in, frame, timeout);
    }

    Status push(Frame& frame, astl::timeout timeout) {
        return io(m_out, frame, timeout);
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
