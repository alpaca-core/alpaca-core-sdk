// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalIoCtx.hpp"
#include "BufferedChannelStream.hpp"
#include "../SessionHandler.hpp"
#include "../IoExecutor.hpp"

namespace ac::frameio {

LocalIoCtx::LocalIoCtx(uint32_t numThreads)
    : m_guard(m_ctx)
    , m_threads(m_ctx, numThreads)
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
    auto strand = m_ctx.make_strand();
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
    if (m_threads.empty()) return;

    m_guard.reset();

    if (forceStop) {
        m_ctx.stop();
    }

    m_threads.join();
}

} // namespace ac::frameio
