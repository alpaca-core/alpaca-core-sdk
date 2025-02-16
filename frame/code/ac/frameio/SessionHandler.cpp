// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "SessionHandler.hpp"
#include "Io.hpp"
#include "IoExecutor.hpp"
#include <cassert>

namespace ac::frameio {

SessionHandler::SessionHandler() noexcept = default;
SessionHandler::~SessionHandler() {
    if (m_successor) {
        assert(m_in && m_out && m_executor.strand);
        init(
            m_successor,
            std::move(m_in),
            std::move(m_out),
            std::move(m_executor)
        );
    }
}

void SessionHandler::shConnected() noexcept {}

void SessionHandler::init(const SessionHandlerPtr& pself,
    InputPtr in,
    OutputPtr out,
    IoExecutor executor
) noexcept {
    auto& self = *pself;
    assert(!self.m_in && !self.m_out && !self.m_executor.strand);
    self.m_in = std::move(in);
    self.m_out = std::move(out);
    self.m_executor = std::move(executor);
    self.m_executor.post([pself] {
        pself->shConnected();
    });
}

} // namespace ac::frameio
