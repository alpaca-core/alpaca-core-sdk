// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "BlockingSyncIoWrapper.hpp"
#include "LocalBufferedChannel.hpp"
#include "LocalChannelUtil.hpp"
#include "BlockingIo.hpp"
#include "SyncIo.hpp"

namespace ac::frameio {

struct BlockingSyncIoWrapper::Impl {
    BlockingIo blockingIo;
    std::function<void()> runTasks;
};

BlockingSyncIoWrapper::BlockingSyncIoWrapper(SessionHandlerPtr handler) {
    auto [elocal, eremote] = LocalChannel_getEndpoints(
        LocalBufferedChannel_create(1),
        LocalBufferedChannel_create(1)
    );

    m_impl = std::make_unique<Impl>(Impl{
        BlockingIo(std::move(elocal)),
        Session_connectSync(std::move(handler), std::move(eremote))
    });
}

BlockingSyncIoWrapper::~BlockingSyncIoWrapper() {
    close();
}

using namespace astl::timeout_vals;

FrameWithStatus BlockingSyncIoWrapper::poll() {
    m_impl->runTasks();
    return m_impl->blockingIo.poll(no_wait);
}

io::status BlockingSyncIoWrapper::poll(Frame& frame) {
    m_impl->runTasks();
    return m_impl->blockingIo.poll(frame, no_wait);
}

io::status BlockingSyncIoWrapper::push(Frame&& frame) {
    auto ret = m_impl->blockingIo.push(std::move(frame), no_wait);
    m_impl->runTasks();
    return ret;
}

io::status BlockingSyncIoWrapper::push(Frame& frame) {
    auto ret = m_impl->blockingIo.push(frame, no_wait);
    m_impl->runTasks();
    return ret;
}

void BlockingSyncIoWrapper::close() {
    m_impl->blockingIo.close();
    m_impl->runTasks();
}

} // namespace ac::frameio
