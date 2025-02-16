// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalIoRunner.hpp"
#include "BufferedChannelStream.hpp"

namespace ac::frameio {

LocalIoRunner::LocalIoRunner(uint32_t numThreads)
    : m_guard(m_ctx)
    , m_threads(m_ctx, numThreads)
{}

LocalIoRunner::~LocalIoRunner() {
    join();
}

ChannelEndpoints LocalIoRunner::getEndpoints(ChannelBufferSizes bufferSizes) {
    return BufferedChannel_getEndpoints(
        BufferedChannel_create(bufferSizes.localToRemote),
        BufferedChannel_create(bufferSizes.remoteToLocal)
    );
}

void LocalIoRunner::connect(SessionHandlerPtr remoteHandler, StreamEndpoint ep) {
    m_ctx.connect(remoteHandler, std::move(ep));
}

StreamEndpoint LocalIoRunner::connect(SessionHandlerPtr remoteHandler, ChannelBufferSizes bufferSizes) {
    auto [local, remote] = getEndpoints(bufferSizes);
    m_ctx.connect(remoteHandler, std::move(remote));
    return std::move(local);
}

void LocalIoRunner::connect(SessionHandlerPtr local, SessionHandlerPtr remote, ChannelBufferSizes bufferSizes) {
    auto [localEp, remoteEp] = getEndpoints(bufferSizes);
    m_ctx.connect(local, std::move(localEp));
    m_ctx.connect(remote, std::move(remoteEp));
}

void LocalIoRunner::join(bool forceStop) {
    if (m_threads.empty()) return;

    m_guard.reset();

    if (forceStop) {
        m_ctx.stop();
    }

    m_threads.join();
}

} // namespace ac::frameio
