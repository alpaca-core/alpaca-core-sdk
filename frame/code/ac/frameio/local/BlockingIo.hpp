// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../../export.h"
#include "../StreamPtr.hpp"
#include "../FrameWithStatus.hpp"
#include <astl/timeout.hpp>

namespace ac::frameio {

struct StreamEndpoint;

// facilitates blocking io, which blocks the current thread until the operation is complete or expires
class AC_FRAME_EXPORT BlockingIo {
public:
    explicit BlockingIo(StreamEndpoint ep);
    BlockingIo(const BlockingIo&) = delete;
    BlockingIo& operator=(const BlockingIo&) = delete;
    BlockingIo(BlockingIo&&) noexcept;
    BlockingIo& operator=(BlockingIo&&) noexcept;
    ~BlockingIo();

    FrameWithStatus poll(astl::timeout timeout = astl::timeout::never());
    io::status poll(Frame& frame, astl::timeout timeout = astl::timeout::never());

    io::status push(Frame&& frame, astl::timeout timeout = astl::timeout::never());
    io::status push(Frame& frame, astl::timeout timeout = astl::timeout::never());

    void close();

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace ac::frameio
