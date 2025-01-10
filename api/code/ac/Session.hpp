// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Frame.hpp"
#include <optional>
#include <functional>

namespace ac {

class AC_API_EXPORT Session {
public:
    virtual ~Session();

    virtual void pushStrandTask(std::function<void()> task) = 0;

    virtual bool hasInFrames() const = 0;
    virtual std::optional<Frame> getInFrame() = 0;

    virtual bool acceptOutFrames() const = 0;
    virtual bool pushOutFrame(Frame&& frame) = 0;

    virtual void close() = 0;
};

class AC_API_EXPORT SessionExecutor {
public:
    virtual ~SessionExecutor();
    virtual void post(std::function<void()> task) = 0;
};

} // namespace ac
