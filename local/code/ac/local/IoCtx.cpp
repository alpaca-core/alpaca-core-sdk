// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "IoCtx.hpp"
#include "Provider.hpp"
#include "ProviderSessionContext.hpp"
#include <ac/frameio/local/BufferedChannelStream.hpp>

#include <ac/xec/context.hpp>
#include <ac/xec/context_work_guard.hpp>
#include <ac/xec/multi_thread_runner.hpp>

namespace ac::local {

using namespace frameio;

struct XCtx {
    xec::context ctx;
    xec::context_work_guard guard;
    xec::multi_thread_runner threads;

    XCtx()
        : guard(ctx)
        , threads(ctx, 1)
    {}

    xec::strand make_strand() {
        return ctx.make_strand();
    }

    void stop(bool force) {
        guard.reset();
        if (force) {
            ctx.stop();
        }
    }
};

struct IoCtx::Impl {
    XCtx dispatch, cpu, gpu;
};

IoCtx::IoCtx()
    : m_impl(std::make_unique<Impl>())
{}

IoCtx::~IoCtx() {
    join();
}

ChannelEndpoints IoCtx::getEndpoints(IoChannelBufferSizes bufferSizes) {
    return BufferedChannel_getEndpoints(
        bufferSizes.localToRemote,
        bufferSizes.remoteToLocal
    );
}

void IoCtx::attach(Provider& provider, frameio::StreamEndpoint remote) {
    ProviderSessionContext ctx = {
        .executor = {
            .dispatch = m_impl->dispatch.make_strand(),
            .cpu = m_impl->cpu.make_strand(),
            .gpu = m_impl->gpu.make_strand(),
        },
        .endpoint = {
            .session = std::move(remote),
            .system = {},
        },
    };
    provider.createSession(std::move(ctx));
}

StreamEndpoint IoCtx::connect(Provider& provider, IoChannelBufferSizes bufferSizes) {
    auto [local, remote] = getEndpoints(bufferSizes);
    attach(provider, std::move(remote));
    return std::move(local);
}

void IoCtx::join(bool forceStop) {
    if (!m_impl) {
        return;
    }

    m_impl->dispatch.stop(forceStop);
    m_impl->cpu.stop(forceStop);
    m_impl->gpu.stop(forceStop);

    m_impl.reset();
}

} // namespace ac::local
