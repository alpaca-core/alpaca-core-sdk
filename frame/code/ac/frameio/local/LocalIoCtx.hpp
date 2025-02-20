// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../../export.h"
#include "../SessionHandlerPtr.hpp"
#include "../StreamEndpoint.hpp"

#include <cstddef>
#include <memory>

namespace ac::frameio {

struct ChannelEndpoints;

// ideally this would be a nested type in LocalIoCtx, but then a clang bug is triggered:
// https://bugs.llvm.org/show_bug.cgi?id=36684
// to work around this, we have the type external
struct ChannelBufferSizes {
    size_t localToRemote = 10;
    size_t remoteToLocal = 10;
};

class AC_FRAME_EXPORT LocalIoCtx {
public:
    explicit LocalIoCtx(uint32_t numThreads = 2);
    LocalIoCtx(const LocalIoCtx&) = delete;
    LocalIoCtx& operator=(const LocalIoCtx&) = delete;
    ~LocalIoCtx();

    static ChannelEndpoints getEndpoints(ChannelBufferSizes bufferSizes = {});

    void connect(SessionHandlerPtr remoteHandler, StreamEndpoint ep);
    StreamEndpoint connect(SessionHandlerPtr remoteHandler, ChannelBufferSizes bufferSizes = {});
    void connect(SessionHandlerPtr local, SessionHandlerPtr remote, ChannelBufferSizes bufferSizes = {});

    void join(bool forceStop = false);
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace ac::frameio
