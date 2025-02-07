// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "SessionCoro.hpp"
#include "SessionHandler.hpp"
#include "IoExecutor.hpp"
#include "StreamEndpoint.hpp"
#include "Io.hpp"
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

Status CoroSessionHandler::getFrame(Input& in, Frame& frame) noexcept {
    return in.get(frame);
}
void CoroSessionHandler::pollFrame(Input& in, Frame& frame, Status& status, astl::timeout timeout) noexcept {
    in.poll(frame, timeout, [this, &status, pl = shared_from_this()](Frame&, Status s) {
        status = s;
        m_currentCoro.resume();
    });
}
Status CoroSessionHandler::putFrame(Output& out, Frame& frame) noexcept {
    return out.put(frame);
}
void CoroSessionHandler::pushFrame(Output& out, Frame& frame, Status& status, astl::timeout timeout) noexcept {
    out.push(frame, timeout, [this, &status, pl = shared_from_this()](Frame&, Status s) {
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

AttachedIo::AttachedIo(CoroSessionHandlerPtr handler, StreamEndpoint ep) {
    m_handler = std::move(handler);
    m_inputPl = m_handler->shExecutor().attachInput(std::move(ep.readStream));
    m_input = m_inputPl.get();
    m_outputPl = m_handler->shExecutor().attachOutput(std::move(ep.writeStream));
    m_output = m_outputPl.get();
}

AttachedIo::~AttachedIo() = default;

AttachedIo::AttachedIo(AttachedIo&&) noexcept = default;
AttachedIo& AttachedIo::operator=(AttachedIo&&) noexcept = default;

AttachedIo Io::attach(StreamEndpoint ep) {
    assert(m_handler);
    return AttachedIo(m_handler, std::move(ep));
}

} // namespace coro

} // namespace ac::frameio
