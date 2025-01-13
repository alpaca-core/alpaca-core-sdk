// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Frame.hpp"
#include "SessionHandlerPtr.hpp"
#include <optional>
#include <functional>

namespace ac {

class SessionExecutor;

class AC_API_EXPORT Session {
public:
    virtual ~Session();

    virtual bool hasInFrames() const = 0;
    virtual std::optional<Frame> getInFrame() = 0;

    virtual bool acceptOutFrames() const = 0;
    virtual bool pushOutFrame(Frame&& frame) = 0;

    virtual void close() = 0;

protected:
    void resetHandler(SessionHandlerPtr handler, std::unique_ptr<SessionExecutor> executor);
    SessionExecutor* executor() const;
    SessionHandlerPtr m_handler;
};

class AC_API_EXPORT SessionExecutor {
public:
    virtual ~SessionExecutor();
    virtual void post(std::function<void()> task) = 0;
};

} // namespace ac
