// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <optional>

namespace ac {

struct Frame {};

class AC_API_EXPORT Session {
public:
    virtual ~Session();

    virtual void pushStrandTask(std::function<void()> task) = 0;

    virtual bool hasInFrames() const = 0;
    virtual std::optional<Frame> getInFrame() = 0;

    virtual bool acceptOutFrames() const = 0;
    virtual bool pushOutFrame(Frame&& frame) = 0;

    virtual bool close();
};

struct SessionExecutor {
    virtual ~SessionExecutor();
    virtual void post(std::function<void()> task) = 0;
};

} // namespace ac
