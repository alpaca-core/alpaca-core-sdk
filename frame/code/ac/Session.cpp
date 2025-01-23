// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Session.hpp"
#include "SessionHandler.hpp"
#include <cassert>

namespace ac {

Session::Session(SessionExecutorPtr executor)
    : m_executor(std::move(executor))
{}

// export vtable
Session::~Session() = default;

void Session::resetHandler(SessionHandlerPtr handler, bool attach) {
    auto oldHandler = std::move(m_handler);
    if (oldHandler) {
        oldHandler->m_session = nullptr;
        oldHandler->shDetached();
    }

    m_handler = std::move(handler);

    if (m_handler) {
        assert(m_handler->m_session == nullptr);
        m_handler->m_session = this;
        if (!m_handler->m_executor) {
            m_handler->m_executor = m_executor;
        }

        // don't change executors!
        assert(m_handler->m_executor == m_executor);

        if (attach) {
            m_handler->shAttached(oldHandler);
        }
    }
}

// just export vtable
SessionExecutor::~SessionExecutor() = default;

} // namespace ac
