// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <ac/Frame.hpp>
#include <ac/SessionHandlerPtr.hpp>
#include <optional>

namespace ac::local {

class AC_LOCAL_EXPORT TestBlockingSession {
public:
    TestBlockingSession(SessionHandlerPtr initialHandler);
    TestBlockingSession(const TestBlockingSession&) = delete;
    TestBlockingSession& operator=(const TestBlockingSession&) = delete;
    ~TestBlockingSession();

    std::optional<Frame> awaitInFrame(int32_t mstimeout = -1);
    void pushOutFrame(Frame&& frame);

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace ac::local
