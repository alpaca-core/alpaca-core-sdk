// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <ac/frameio/FrameWithStatus.hpp>
#include <ac/frameio/SessionHandlerPtr.hpp>
#include <ac/xec/strand.hpp>

namespace ac::local {

// wraps blocking and sync io to make local testing and experimentation easier
class AC_LOCAL_EXPORT BlockingSyncIoHelper {
public:
    explicit BlockingSyncIoHelper(frameio::SessionHandlerPtr handler);
    BlockingSyncIoHelper(const BlockingSyncIoHelper&) = delete;
    BlockingSyncIoHelper& operator=(const BlockingSyncIoHelper&) = delete;
    BlockingSyncIoHelper(BlockingSyncIoHelper&&) noexcept;
    BlockingSyncIoHelper& operator=(BlockingSyncIoHelper&&) noexcept;
    ~BlockingSyncIoHelper();

    xec::strand makeSyncStrand();

    frameio::FrameWithStatus poll();
    io::status poll(Frame& frame);

    io::status push(Frame&& frame);
    io::status push(Frame& frame);

    void close();

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace ac::local
