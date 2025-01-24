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
    pushCoro(sh, std::exchange(coro.m_handle, nullptr));
    return sh;
}

void CoroSessionHandler::postResume() {
    shExecutor().post([this, pl = shared_from_this()] {
        m_currentCoro.resume();
    });
}

void CoroSessionHandler::pollFrame(Frame& frame, Status& status, int32_t timeout) noexcept {
    shInput().poll(frame, timeout, [this, &status, pl = shared_from_this()](Frame&, Status s) {
        status = s;
        m_currentCoro.resume();
    });
}
void CoroSessionHandler::pushFrame(Frame& frame, Status& status, int32_t timeout) noexcept {
    shOutput().push(frame, timeout, [this, &status, pl = shared_from_this()](Frame&, Status s) {
        status = s;
        m_currentCoro.resume();
    });
}
void CoroSessionHandler::close() {
    shInput().close();
    shOutput().close();
}

void CoroSessionHandler::shConnected() noexcept {
    assert(m_currentCoro);
    m_currentCoro.resume();
}

} // namespace ac::frameio
