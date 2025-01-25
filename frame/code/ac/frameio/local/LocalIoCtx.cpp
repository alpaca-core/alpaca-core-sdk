// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalIoCtx.hpp"
#include "../SessionHandler.hpp"
#include "../IoExecutor.hpp"
#include "../FrameWithStatus.hpp"
#include "../Io.hpp"
#include "../BasicStreamIo.hpp"
#include "../IoCommon.hpp"
#include "../StreamEndpoint.hpp"

#include <boost/asio/strand.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/io_context.hpp>

namespace asio = boost::asio;

namespace ac::frameio {

namespace {

using Strand = asio::strand<asio::io_context::executor_type>;

class ETimer : public asio::steady_timer {
public:
    using asio::steady_timer::basic_waitable_timer;
    void expires_after_ms(astl::timeout timeout) {
        if (timeout.infinite()) {
            expires_at(asio::steady_timer::time_point::max());
        }
        else {
            expires_after(timeout.duration);
        }
    }
};

struct StrandIo : public BasicStreamIo {
    Strand m_strand;
    ETimer m_timer;

    StrandIo(StreamPtr&& stream, const Strand& strand)
        : BasicStreamIo(std::move(stream))
        , m_strand(strand)
        , m_timer(strand)
    {}

    FrameRefWithStatus io(Frame& frame) {
        auto status = m_stream->stream(frame, nullptr);
        return FrameRefWithStatus(frame, status);
    }

    void io(Frame& frame, astl::timeout timeout, IoCb cb) {
        {
            auto status = m_stream->stream(frame, [this]() {
                return [this]() {
                    asio::post(m_strand, [this]() {
                        m_timer.cancel();
                    });
                };
            });

            if (status.complete()) {
                asio::post(m_strand, [this, status, &frame, cb = std::move(cb)]() {
                    cb(frame, status);
                });
                return;
            }
        }

        m_timer.expires_after_ms(timeout);
        m_timer.async_wait([this, &frame, cb = std::move(cb)](const boost::system::error_code& ec) {
            auto status = m_stream->stream(frame, nullptr);

            if (ec == asio::error::operation_aborted) {
                status.setAborted();
            }
            else {
                status.setTimeout();
            }

            cb(frame, status);
        });
    }
};

using StrandInput = InputCommon<StrandIo>;
using StrandOutput = OutputCommon<StrandIo>;

struct StrandExecutor final : public IoExecutor {
    Strand m_strand;
    StrandExecutor(const Strand& strand)
        : m_strand(strand)
    {}
    virtual void post(std::function<void()> task) override {
        asio::post(m_strand, std::move(task));
    }
};

} // namespace

class LocalIoCtx::Impl {
public:
    asio::io_context m_ctx;
    asio::executor_work_guard<asio::io_context::executor_type> m_workGuard;

    Impl()
        : m_workGuard(asio::make_work_guard(m_ctx))
    {}

    void run() {
        m_ctx.run();
    }

    void forceStop() {
        m_ctx.stop();
    }

    void complete() {
        m_ctx.post([this]() {
            m_workGuard.reset();
        });
    }
};

LocalIoCtx::LocalIoCtx()
    : m_impl(std::make_unique<Impl>())
{}

LocalIoCtx::~LocalIoCtx() = default;

void LocalIoCtx::run() {
    m_impl->run();
}

void LocalIoCtx::forceStop() {
    m_impl->forceStop();
}

void LocalIoCtx::complete() {
    m_impl->complete();
}

void LocalIoCtx::connect(SessionHandlerPtr handler, StreamEndpoint ep) {
    Strand strand(m_impl->m_ctx.get_executor());
    asio::post(strand, [strand, handler, ep = std::move(ep)]() mutable {
        SessionHandler::init(
            *handler,
            std::make_unique<StrandInput>(std::move(ep.readStream), strand),
            std::make_unique<StrandOutput>(std::move(ep.writeStream), strand),
            std::make_unique<StrandExecutor>(strand)
        );
    });
}

} // namespace ac::frameio
