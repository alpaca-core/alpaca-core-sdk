// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <ac/SessionHandlerPtr.hpp>
#include <optional>

namespace ac {
struct Frame;
}

namespace ac::local {

class LocalConnection;

class BlockingSession {
    friend class LocalConnection;
    BlockingSession();
public:
    BlockingSession(const BlockingSession&) = delete;
    BlockingSession& operator=(const BlockingSession&) = delete;
    BlockingSession(BlockingSession&&) noexcept;
    BlockingSession& operator=(BlockingSession&&) noexcept;
    ~BlockingSession();

    std::optional<Frame> awaitInFrame(int32_t mstimeout = -1);

    void pushOutFrame(Frame&& frame);

    void suspendRemote();

    void close();

    bool connected() const noexcept { return !!m_impl; }
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

class AC_LOCAL_EXPORT LocalConnection {
public:
    LocalConnection();
    LocalConnection(const LocalConnection&) = delete;
    LocalConnection& operator=(const LocalConnection&) = delete;
    ~LocalConnection();

    void run();

    void connectHandlers(SessionHandlerPtr a, SessionHandlerPtr b);

    BlockingSession connectSession(SessionHandlerPtr remote);

    void stop();
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace ac::local
