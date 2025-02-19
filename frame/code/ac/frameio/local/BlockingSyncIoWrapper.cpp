// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "BlockingSyncIoWrapper.hpp"
#include "BufferedChannel.hpp"
#include "BufferedChannelStream.hpp"
#include "BlockingIo.hpp"
#include "../SessionHandler.hpp"
#include "../IoExecutor.hpp"
#include <ac/xec/context.hpp>

namespace ac::frameio {

struct BlockingSyncIoWrapper::Impl {
    Impl(StreamEndpoint ep)
        : blockingIo(std::move(ep))
    {}

    BlockingIo blockingIo;
    xec::context syncCtx;

    void runSyncTasks() {
        syncCtx.poll();
    }
};

BlockingSyncIoWrapper::BlockingSyncIoWrapper(SessionHandlerPtr handler) {
    auto [elocal, eremote] = BufferedChannel_getEndpoints(1, 1);

    m_impl = std::make_unique<Impl>(std::move(elocal));

    IoExecutor ex(m_impl->syncCtx.make_strand());
    SessionHandler::init(
        handler,
        ex.attachInput(std::move(eremote.read_stream)),
        ex.attachOutput(std::move(eremote.write_stream)),
        ex
    );
}

BlockingSyncIoWrapper::~BlockingSyncIoWrapper() {
    close();
}

xec::strand BlockingSyncIoWrapper::makeSyncStrand() {
    return m_impl->syncCtx.make_strand();
}

using namespace astl::timeout_vals;

FrameWithStatus BlockingSyncIoWrapper::poll() {
    m_impl->runSyncTasks();
    return m_impl->blockingIo.poll(no_wait);
}

io::status BlockingSyncIoWrapper::poll(Frame& frame) {
    m_impl->runSyncTasks();
    return m_impl->blockingIo.poll(frame, no_wait);
}

io::status BlockingSyncIoWrapper::push(Frame&& frame) {
    auto ret = m_impl->blockingIo.push(std::move(frame), no_wait);
    m_impl->runSyncTasks();
    return ret;
}

io::status BlockingSyncIoWrapper::push(Frame& frame) {
    auto ret = m_impl->blockingIo.push(frame, no_wait);
    m_impl->runSyncTasks();
    return ret;
}

void BlockingSyncIoWrapper::close() {
    m_impl->blockingIo.close();
    m_impl->runSyncTasks();
}

} // namespace ac::frameio
