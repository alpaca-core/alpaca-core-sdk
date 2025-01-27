// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "SessionCoro.hpp"
#include "SessionHandler.hpp"
#include "Io.hpp"
#include "IoExecutor.hpp"
#include <cassert>

namespace ac::frameio {

CoroSessionHandler::~CoroSessionHandler() = default;

SessionHandlerPtr CoroSessionHandler::create(SessionCoro<void> coro) {
    auto sh = std::make_shared<CoroSessionHandler>();
    pushCoro(sh, coro.takeHandle());
    return sh;
}

void CoroSessionHandler::postResume() noexcept {
    shExecutor().post([this, pl = shared_from_this()] {
        m_currentCoro.resume();
    });
}

FrameRefWithStatus CoroSessionHandler::getFrame(Frame& frame) noexcept {
    return shInput().get(frame);
}
void CoroSessionHandler::pollFrame(Frame& frame, Status& status, astl::timeout timeout) noexcept {
    shInput().poll(frame, timeout, [this, &status, pl = shared_from_this()](Frame&, Status s) {
        status = s;
        m_currentCoro.resume();
    });
}
FrameRefWithStatus CoroSessionHandler::putFrame(Frame& frame) noexcept {
    return shOutput().put(frame);
}
void CoroSessionHandler::pushFrame(Frame& frame, Status& status, astl::timeout timeout) noexcept {
    shOutput().push(frame, timeout, [this, &status, pl = shared_from_this()](Frame&, Status s) {
        status = s;
        m_currentCoro.resume();
    });
}
void CoroSessionHandler::close() noexcept {
    shInput().close();
    shOutput().close();
}

void CoroSessionHandler::shConnected() noexcept {
    assert(m_currentCoro);
    m_currentCoro.resume();
}

namespace coro {
// export the vtable
IoClosed::~IoClosed() = default;
}

} // namespace ac::frameio
