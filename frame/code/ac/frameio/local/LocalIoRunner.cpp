// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalIoRunner.hpp"
#include "BlockingIo.hpp"
#include "LocalBufferedChannel.hpp"
#include "LocalChannelUtil.hpp"

namespace ac::frameio {

LocalIoRunner::LocalIoRunner(uint32_t numThreads)
    : m_threads(numThreads)
{
    for (auto& t : m_threads) {
        t = std::thread([this] { m_ctx.run(); });
    }
}

LocalIoRunner::~LocalIoRunner() {
    join();
}

LocalEndpoints LocalIoRunner::getEndpoints(ChannelBufferSizes bufferSizes) {
    return LocalChannel_getEndpoints(
        LocalBufferedChannel_create(bufferSizes.localToRemote),
        LocalBufferedChannel_create(bufferSizes.remoteToLocal)
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

BlockingIo LocalIoRunner::connectBlocking(SessionHandlerPtr remoteHandler, ChannelBufferSizes bufferSizes) {
    return BlockingIo(connect(remoteHandler, bufferSizes));
}

void LocalIoRunner::connect(SessionHandlerPtr local, SessionHandlerPtr remote, ChannelBufferSizes bufferSizes) {
    auto [localEp, remoteEp] = getEndpoints(bufferSizes);
    m_ctx.connect(local, std::move(localEp));
    m_ctx.connect(remote, std::move(remoteEp));
}

void LocalIoRunner::join(bool forceStop) {
    if (m_threads.empty()) return;

    if (forceStop) {
        m_ctx.forceStop();
    }
    else {
        m_ctx.complete();
    }

    for (auto& t : m_threads) {
        t.join();
    }
    m_threads.clear();
}

} // namespace ac::frameio
