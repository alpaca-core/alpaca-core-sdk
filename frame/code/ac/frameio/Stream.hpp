// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../export.h"
#include "../Frame.hpp"
#include <ac/io/stream_result.hpp>

namespace ac::frameio {

using Stream_OnBlockedFunc = astl::ufunction<xec::task()>;

class AC_FRAME_EXPORT ReadStream {
public:
    using read_value_type = Frame;
    using value_type = Frame;

    virtual ~ReadStream();
    virtual io::stream_result read(Frame& f, Stream_OnBlockedFunc onBlocked) = 0;
    virtual void close() = 0;
};

class AC_FRAME_EXPORT WriteStream {
public:
    using write_value_type = Frame;
    using value_type = Frame;

    virtual ~WriteStream();
    virtual io::stream_result write(Frame& f, Stream_OnBlockedFunc onBlocked) = 0;
    virtual void close() = 0;
};

} // namespace ac::frameio
