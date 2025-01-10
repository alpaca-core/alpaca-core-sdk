// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "SessionHandler.hpp"
#include "Session.hpp"
#include <astl/move.hpp>

namespace ac {

void SessionHandler::pushSessionStrandTask(std::function<void()> task) {
    m_executor->post(astl::move(task));
}

bool SessionHandler::sessionHasInFrames() const {
    if (!m_session) return false;
    return m_session->hasInFrames();
}

std::optional<Frame> SessionHandler::getSessionInFrame() {
    if (!m_session) return {};
    return m_session->getInFrame();
}

bool SessionHandler::sessionAcceptsOutFrames() const {
    if (!m_session) return false;
    return m_session->acceptOutFrames();
}

bool SessionHandler::pushSessionOutFrame(Frame&& frame) {
    if (!m_session) return false;
    return m_session->pushOutFrame(astl::move(frame));
}

void SessionHandler::closeSession() {
    if (!m_session) return;
    m_session->close();
}

} // namespace ac
