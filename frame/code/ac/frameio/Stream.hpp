// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../export.h"
#include "../Frame.hpp"
#include <ac/io/status.hpp>
#include <functional>

namespace ac::frameio {

class AC_FRAME_EXPORT Stream {
public:
    using NotifyCb = std::function<void()>;
    using OnBlockedFunc = std::function<NotifyCb()>;

    virtual ~Stream();
    virtual io::status stream(Frame& f, OnBlockedFunc onBlocked) = 0;
    virtual void close() = 0;

    // possible functions which are not implemented
    // they would only make sense if a stream is used from multiple threads
    // there is no use case for that yet
    //
    // allow a stream op to take over the wait state of the stream
    // optionally trigger a spurious wakeup of the previous waiter if any
    // default arg is equivalent to current implementation
    // virtual io::status stream(Frame& f, OnBlockedFunc onBlocked, bool callPrevious = false) = 0;
    //
    // take over the waiting state of the stream without a stream op
    // optionally trigger a spurious wakeup of the previous waiter if any
    // virtual io::status replaceWaiter(OnBlockedFunc onBlocked, bool callPrevious = false) = 0;
};

class AC_FRAME_EXPORT ReadStream : public Stream {
public:
    virtual ~ReadStream();
    virtual io::status read(Frame& f, OnBlockedFunc onBlocked) = 0;

    io::status stream(Frame& f, OnBlockedFunc onBlocked) final override {
        return read(f, std::move(onBlocked));
    }
};

class AC_FRAME_EXPORT WriteStream : public Stream {
public:
    virtual ~WriteStream();
    virtual io::status write(Frame& f, OnBlockedFunc onBlocked) = 0;

    io::status stream(Frame& f, OnBlockedFunc onBlocked) final override {
        return write(f, std::move(onBlocked));
    }
};

} // namespace ac::frameio
