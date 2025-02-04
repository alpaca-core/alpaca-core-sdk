// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "SessionCoro.hpp"
#include "SessionHandler.hpp"
#include "Io.hpp"
#include "IoExecutor.hpp"
#include <cassert>

namespace ac::frameio {

CoroSessionHandler::~CoroSessionHandler() {
    if (m_sucessorResult.has_error()) {
        // can't propagate the exception to the caller
        std::terminate();
    }
    auto s = std::move(m_sucessorResult.value());

    if (s) {
        setSuccessor(std::move(s));
    }
    else {
        close();
    }
}

template <typename PromiseType>
CoroSessionHandlerPtr CoroSessionHandler::doCreate(std::coroutine_handle<PromiseType> h) {
    auto sh = std::make_shared<CoroSessionHandler>();
    h.promise().m_handler = sh;
    sh->setCoro(h);
    return sh;
}

SessionHandlerPtr CoroSessionHandler::create(SessionCoro<void> coro) {
    return doCreate(coro.takeHandle());
}

SessionHandlerPtr CoroSessionHandler::create(SessionCoro<SessionHandlerPtr> coro) {
    auto h = coro.takeHandle();
    auto self = doCreate(h);
    h.promise().m_result = &self->m_sucessorResult;
    return self;
}

void CoroSessionHandler::postResume() noexcept {
    shExecutor().post([this, pl = shared_from_this()] {
        m_currentCoro.resume();
    });
}

Status CoroSessionHandler::getFrame(Frame& frame) noexcept {
    return shInput().get(frame);
}
void CoroSessionHandler::pollFrame(Frame& frame, Status& status, astl::timeout timeout) noexcept {
    shInput().poll(frame, timeout, [this, &status, pl = shared_from_this()](Frame&, Status s) {
        status = s;
        m_currentCoro.resume();
    });
}
Status CoroSessionHandler::putFrame(Frame& frame) noexcept {
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

} // namespace ac::frameio
