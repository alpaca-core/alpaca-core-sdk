// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../../export.h"
#include "../FrameWithStatus.hpp"
#include "../SessionHandlerPtr.hpp"

namespace ac::frameio {

// wraps blocknd and sync io to make local testing and experimentation easier
class AC_FRAME_EXPORT BlockingSyncIoWrapper {
public:
    explicit BlockingSyncIoWrapper(SessionHandlerPtr handler);
    BlockingSyncIoWrapper(const BlockingSyncIoWrapper&) = delete;
    BlockingSyncIoWrapper& operator=(const BlockingSyncIoWrapper&) = delete;
    BlockingSyncIoWrapper(BlockingSyncIoWrapper&&) noexcept;
    BlockingSyncIoWrapper& operator=(BlockingSyncIoWrapper&&) noexcept;
    ~BlockingSyncIoWrapper();

    FrameWithStatus poll();
    FrameRefWithStatus poll(Frame& frame);

    Status push(Frame&& frame);
    FrameRefWithStatus push(Frame& frame);

    void close();

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace ac::frameio
