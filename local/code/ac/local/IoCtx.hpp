// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <ac/frameio/StreamEndpoint.hpp>

#include <cstddef>
#include <memory>

namespace ac::frameio {
struct ChannelEndpoints;
}

namespace ac::local {

class Provider;

// ideally this would be a nested type in IoCtx, but then a clang bug is triggered:
// https://bugs.llvm.org/show_bug.cgi?id=36684
// to work around this, we have the type external
struct IoChannelBufferSizes {
    size_t localToRemote = 10;
    size_t remoteToLocal = 10;
};

class AC_LOCAL_EXPORT IoCtx {
public:
    IoCtx();
    IoCtx(const IoCtx&) = delete;
    IoCtx& operator=(const IoCtx&) = delete;
    ~IoCtx();

    static frameio::ChannelEndpoints getEndpoints(IoChannelBufferSizes bufferSizes = {});

    frameio::StreamEndpoint connect(Provider& provider, IoChannelBufferSizes bufferSizes = {});

    void join(bool forceStop = false);
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace ac::local
