// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Frame.hpp"
#include "SessionExecutorPtr.hpp"
#include "SessionHandlerPtr.hpp"
#include <optional>
#include <functional>

namespace ac {

class AC_API_EXPORT Session {
public:
    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;

    // sync queries
    virtual std::optional<Frame> getInFrame() = 0;

    virtual bool acceptOutFrames() const = 0;
    virtual bool pushOutFrame(Frame&& frame) = 0;

    // async queries
    virtual void pollInFramesAvailable() = 0;
    virtual void pollOutFramesAccepted() = 0;

    virtual void close() = 0;

    void resetHandler(SessionHandlerPtr handler, bool attach = true);

    const SessionHandlerPtr& handler() const { return m_handler; }
    const SessionExecutorPtr& executor() const { return m_executor; }

protected:
    Session(SessionExecutorPtr executor);
    virtual ~Session(); // only virtual to export the vtable

    SessionExecutorPtr m_executor;
    SessionHandlerPtr m_handler;
};

class AC_API_EXPORT SessionExecutor {
public:
    virtual ~SessionExecutor();
    virtual void post(std::function<void()> task) = 0;
};

} // namespace ac
