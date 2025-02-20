// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalIoCtx.hpp"
#include "BufferedChannelStream.hpp"
#include "../SessionHandler.hpp"
#include "../IoExecutor.hpp"

#include <ac/xec/context.hpp>
#include <ac/xec/context_work_guard.hpp>
#include <astl/multi_thread_runner.hpp>


namespace ac::frameio {

struct LocalIoCtx::Impl {
    xec::context ctx;
    ac::xec::context_work_guard guard;
    astl::multi_thread_runner threads;
    Impl(uint32_t numThreads)
        : guard(ctx)
        , threads(ctx, numThreads)
    {}
};

LocalIoCtx::LocalIoCtx(uint32_t numThreads)
    : m_impl(std::make_unique<Impl>(numThreads))
{}

LocalIoCtx::~LocalIoCtx() {
    join();
}

ChannelEndpoints LocalIoCtx::getEndpoints(ChannelBufferSizes bufferSizes) {
    return BufferedChannel_getEndpoints(
        bufferSizes.localToRemote,
        bufferSizes.remoteToLocal
    );
}

void LocalIoCtx::connect(SessionHandlerPtr handler, StreamEndpoint ep) {
    auto strand = m_impl->ctx.make_strand();
    IoExecutor executor{ strand };
    SessionHandler::init(
        handler,
        executor.attachInput(std::move(ep.read_stream)),
        executor.attachOutput(std::move(ep.write_stream)),
        executor
    );
}

StreamEndpoint LocalIoCtx::connect(SessionHandlerPtr remoteHandler, ChannelBufferSizes bufferSizes) {
    auto [local, remote] = getEndpoints(bufferSizes);
    connect(remoteHandler, std::move(remote));
    return std::move(local);
}

void LocalIoCtx::connect(SessionHandlerPtr local, SessionHandlerPtr remote, ChannelBufferSizes bufferSizes) {
    auto [localEp, remoteEp] = getEndpoints(bufferSizes);
    connect(local, std::move(localEp));
    connect(remote, std::move(remoteEp));
}

void LocalIoCtx::join(bool forceStop) {
    if (m_impl->threads.empty()) return;

    m_impl->guard.reset();

    if (forceStop) {
        m_impl->ctx.stop();
    }

    m_impl->threads.join();
}

} // namespace ac::frameio
