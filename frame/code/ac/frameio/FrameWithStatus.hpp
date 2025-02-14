// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../Frame.hpp"
#include <ac/io/status.hpp>

namespace ac::frameio {

struct FrameWithStatus : public io::status {
    FrameWithStatus() = default;
    explicit FrameWithStatus(Frame frame) : frame(std::move(frame)) {}
    FrameWithStatus(Frame frame, io::status status) : io::status(status), frame(std::move(frame)) {}

    io::status& status() { return *this; }
    const io::status& status() const { return *this; }

    Frame frame = {};

    void reset(Frame f = {}) {
        io::status::reset();
        frame = std::move(f);
    }
};

struct FrameRefWithStatus : public io::status {
    explicit FrameRefWithStatus(Frame& frame) : frame(frame) {}
    FrameRefWithStatus(Frame& frame, io::status status) : io::status(status), frame(frame) {}

    io::status& status() { return *this; }
    const io::status& status() const { return *this; }

    Frame& frame;
};

} // namespace ac
