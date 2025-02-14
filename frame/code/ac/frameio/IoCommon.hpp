// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../export.h"
#include "Io.hpp"

namespace ac::frameio {

template <typename IO>
struct InputCommon final : public IO, public Input {
    using IO::IO;
    virtual io::status get(Frame& frame) override { return IO::io(frame); }
    virtual void poll(Frame& frame, astl::timeout timeout, IoCb cb) override { IO::io(frame, timeout, std::move(cb)); }
    virtual void close() override { IO::close(); }
};

template <typename IO>
struct OutputCommon final : public IO, public Output {
    using IO::IO;
    virtual io::status put(Frame& frame) override { return IO::io(frame); }
    virtual void push(Frame& frame, astl::timeout timeout, IoCb cb) override { IO::io(frame, timeout, std::move(cb)); }
    virtual void close() override { IO::close(); }
};

} // namespace ac::frameio
