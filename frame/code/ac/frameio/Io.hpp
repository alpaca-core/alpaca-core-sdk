// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../export.h"
#include "FrameWithStatus.hpp"
#include <astl/timeout.hpp>

namespace ac::frameio {

using IoCb = std::function<void(Frame&, io::status)>;

class AC_FRAME_EXPORT Input {
public:
    virtual ~Input();
    virtual io::status get(Frame& frame) = 0;
    virtual void poll(Frame& frame, astl::timeout timeout, IoCb cb) = 0;
    virtual void close() = 0;
};

class AC_FRAME_EXPORT Output {
public:
    virtual ~Output();
    virtual io::status put(Frame& frame) = 0;
    virtual void push(Frame& frame, astl::timeout timeout, IoCb cb) = 0;
    virtual void close() = 0;
};

} // namespace ac::frameio
