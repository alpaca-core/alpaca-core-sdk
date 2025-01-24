// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../Frame.hpp"
#include "Status.hpp"

namespace ac::frameio {

struct FrameWithStatus : public Status {
    FrameWithStatus() = default;
    explicit FrameWithStatus(Frame frame) : frame(std::move(frame)) {}
    FrameWithStatus(Frame frame, Status status) : Status(status), frame(std::move(frame)) {}

    Status& status() { return *this; }
    const Status& status() const { return *this; }

    Frame frame = {};

    void reset(Frame f = {}) {
        Status::reset();
        frame = std::move(f);
    }
};

struct FrameRefWithStatus : public Status {
    explicit FrameRefWithStatus(Frame& frame) : frame(frame) {}
    FrameRefWithStatus(Frame& frame, Status status) : Status(status), frame(frame) {}

    Status& status() { return *this; }
    const Status& status() const { return *this; }

    Frame& frame;
};

} // namespace ac
