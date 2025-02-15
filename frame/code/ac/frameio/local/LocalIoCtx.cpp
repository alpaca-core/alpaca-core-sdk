// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalIoCtx.hpp"
#include "../SessionHandler.hpp"
#include "../FrameWithStatus.hpp"
#include "../BasicStreamIo.hpp"
#include "../IoCommon.hpp"
#include "../StreamEndpoint.hpp"
#include "../IoExecutor.hpp"

#include <ac/xec/strand.hpp>
#include <ac/xec/timer.hpp>
#include <ac/xec/context.hpp>
#include <ac/xec/context_work_guard.hpp>
#include <ac/xec/post.hpp>

namespace ac::frameio {

namespace {

struct StrandIo : public BasicStreamIo {
    xec::strand m_strand;
    xec::timer_ptr m_timer;

    StrandIo(StreamPtr&& stream, const xec::strand& strand)
        : BasicStreamIo(std::move(stream))
        , m_strand(strand)
        , m_timer(xec::timer::create(strand))
    {}

    io::status io(Frame& frame) {
        return m_stream->stream(frame, nullptr);
    }

    void io(Frame& frame, astl::timeout timeout, IoCb cb) {
        {
            auto status = m_stream->stream(frame, [this]() {
                return [this]() {
                    post(m_strand, [this]() {
                        m_timer->cancel();
                    });
                };
            });

            if (status.complete()) {
                post(m_strand, [status, &frame, cb = std::move(cb)]() {
                    cb(frame, status);
                });
                return;
            }
        }

        m_timer->set_timeout(timeout);
        m_timer->add_wait_cb([this, &frame, cb = std::move(cb)](const std::error_code& ec) {
            auto status = m_stream->stream(frame, nullptr);

            if (ec) {
                status.set_aborted();
            }
            else {
                status.set_timeout();
            }

            cb(frame, status);
        });
    }
};

using StrandInput = InputCommon<StrandIo>;
using StrandOutput = OutputCommon<StrandIo>;

struct StrandExecutor final : public IoExecutor {
    xec::strand m_strand;
    explicit StrandExecutor(const xec::strand& strand)
        : m_strand(strand)
    {}
    virtual void post(std::function<void()> task) override {
        xec::post(m_strand, std::move(task));
    }
    virtual InputPtr attachInput(ReadStreamPtr stream) override {
        return std::make_unique<StrandInput>(std::move(stream), m_strand);
    }
    virtual OutputPtr attachOutput(WriteStreamPtr stream) override {
        return std::make_unique<StrandOutput>(std::move(stream), m_strand);
    }
};

} // namespace

class LocalIoCtx::Impl {
public:
    xec::context m_ctx;
    xec::context_work_guard m_workGuard;

    Impl()
        : m_workGuard(m_ctx)
    {}

    void run() {
        m_ctx.run();
    }

    void forceStop() {
        m_ctx.stop();
    }

    void complete() {
        post(m_ctx, [this]() {
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
    auto strand = m_impl->m_ctx.make_strand();
    auto executor = std::make_shared<StrandExecutor>(strand);
    SessionHandler::init(
        handler,
        executor->attachInput(std::move(ep.readStream)),
        executor->attachOutput(std::move(ep.writeStream)),
        executor
    );
}

} // namespace ac::frameio
