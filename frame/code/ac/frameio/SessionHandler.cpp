// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "SessionHandler.hpp"
#include "Io.hpp"
#include "IoExecutor.hpp"
#include <cassert>

namespace ac::frameio {

SessionHandler::SessionHandler() noexcept = default;
SessionHandler::~SessionHandler() = default;

void SessionHandler::shConnected() noexcept {}

void SessionHandler::init(SessionHandler& self,
    InputPtr in,
    OutputPtr out,
    IoExecutorPtr executor
) noexcept {
    assert(!self.m_in && !self.m_out && !self.m_executor);
    self.m_in = std::move(in);
    self.m_out = std::move(out);
    self.m_executor = std::move(executor);
    self.shConnected();
}

} // namespace ac::frameio
