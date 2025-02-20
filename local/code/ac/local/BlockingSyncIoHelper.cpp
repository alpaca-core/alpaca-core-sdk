// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "BlockingSyncIoHelper.hpp"
#include "Provider.hpp"
#include "ProviderSessionContext.hpp"
#include <ac/frameio/local/BufferedChannel.hpp>
#include <ac/frameio/local/BufferedChannelStream.hpp>
#include <ac/frameio/local/BlockingIo.hpp>

#include <ac/xec/context.hpp>

namespace ac::local {

using namespace frameio;

struct BlockingSyncIoHelper::Impl {
    Impl(StreamEndpoint ep)
        : blockingIo(std::move(ep))
    {}

    BlockingIo blockingIo;
    xec::context syncCtx;

    void runSyncTasks() {
        syncCtx.poll();
    }
};

BlockingSyncIoHelper::BlockingSyncIoHelper(Provider& provider) {
    auto [elocal, eremote] = BufferedChannel_getEndpoints(1, 1);

    m_impl = std::make_unique<Impl>(std::move(elocal));

    ProviderSessionContext ctx = {
        .executor = {
            .dispatch = m_impl->syncCtx.make_strand(),
            .cpu = m_impl->syncCtx.make_strand(),
            .gpu = m_impl->syncCtx.make_strand(),
        },
        .endpoint = {
            .session = std::move(eremote),
            .system = {},
        },
    };

    provider.createSession(std::move(ctx));
}

BlockingSyncIoHelper::~BlockingSyncIoHelper() {
    close();
}

xec::strand BlockingSyncIoHelper::makeSyncStrand() {
    return m_impl->syncCtx.make_strand();
}

using namespace astl::timeout_vals;

FrameWithStatus BlockingSyncIoHelper::poll() {
    m_impl->runSyncTasks();
    return m_impl->blockingIo.poll(no_wait);
}

io::status BlockingSyncIoHelper::poll(Frame& frame) {
    m_impl->runSyncTasks();
    return m_impl->blockingIo.poll(frame, no_wait);
}

io::status BlockingSyncIoHelper::push(Frame&& frame) {
    auto ret = m_impl->blockingIo.push(std::move(frame), no_wait);
    m_impl->runSyncTasks();
    return ret;
}

io::status BlockingSyncIoHelper::push(Frame& frame) {
    auto ret = m_impl->blockingIo.push(frame, no_wait);
    m_impl->runSyncTasks();
    return ret;
}

void BlockingSyncIoHelper::close() {
    m_impl->blockingIo.close();
    m_impl->runSyncTasks();
}

} // namespace ac::local
