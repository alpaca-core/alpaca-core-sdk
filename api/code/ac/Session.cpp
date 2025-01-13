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

void Session::resetHandler(SessionHandlerPtr handler) {
    if (m_handler) {
        m_handler->m_session = nullptr;
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

        // not necessarily opened yet
    }
}

// just export vtable
SessionExecutor::~SessionExecutor() = default;

} // namespace ac
