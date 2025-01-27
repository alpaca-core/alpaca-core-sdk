// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../../export.h"
#include "LocalIoCtx.hpp"
#include "../SessionHandlerPtr.hpp"

#include <thread>
#include <vector>
#include <cstdint>

namespace ac::frameio {

class BlockingIo;
struct LocalEndpoints;

class AC_FRAME_EXPORT LocalIoRunner {
public:
    explicit LocalIoRunner(uint32_t numThreads = 2);
    LocalIoRunner(const LocalIoRunner&) = delete;
    LocalIoRunner& operator=(const LocalIoRunner&) = delete;
    ~LocalIoRunner();

    struct ChannelBufferSizes {
        size_t localToRemote = 10;
        size_t remoteToLocal = 10;
    };

    LocalEndpoints getEndpoints(ChannelBufferSizes bufferSizes = {});

    BlockingIo connectBlocking(SessionHandlerPtr remoteHandler, ChannelBufferSizes bufferSizes = {});
    void connect(SessionHandlerPtr local, SessionHandlerPtr remote, ChannelBufferSizes bufferSizes = {});

    void join(bool forceStop = false);

    LocalIoCtx& ctx() { return m_ctx; }

private:
    LocalIoCtx m_ctx;
    std::vector<std::thread> m_threads;
};

} // namespace ac::frameio
