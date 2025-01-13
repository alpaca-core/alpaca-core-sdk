// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Session.hpp"
#include "SessionHandler.hpp"
#include <cassert>

namespace ac {
// export vtable
Session::~Session() = default;

void Session::resetHandler(SessionHandlerPtr handler, std::unique_ptr<SessionExecutor> executor) {
    if (m_handler) {
        m_handler->shClosed();
        m_handler->m_session = nullptr;
    }

    m_handler = std::move(handler);

    if (m_handler) {
        assert(m_handler->m_session == nullptr);
        m_handler->m_session = this;
        m_handler->m_executor = std::move(executor);
    }
}

SessionExecutor* Session::executor() const {
    if (!m_handler) return nullptr;
    return m_handler->m_executor.get();
}

// just export vtable
SessionExecutor::~SessionExecutor() = default;

} // namespace ac
