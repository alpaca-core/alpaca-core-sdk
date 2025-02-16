// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../export.h"
#include "../Frame.hpp"
#include <ac/io/value_with_status.hpp>
#include <ac/io/stream_result.hpp>

namespace ac::frameio {

class AC_FRAME_EXPORT Stream {
public:
    virtual ~Stream();
    virtual io::stream_result stream(Frame& f, xec::notifiable* nobj) = 0;
    virtual void close() = 0;
};

class AC_FRAME_EXPORT ReadStream : public Stream {
public:
    using read_value_type = Frame;
    using value_type = Frame;

    virtual ~ReadStream();
    virtual io::stream_result read(Frame& f, xec::notifiable* nobj) = 0;

    io::stream_result stream(Frame& f, xec::notifiable* nobj) final override {
        return read(f, nobj);
    }
};

class AC_FRAME_EXPORT WriteStream : public Stream {
public:
    using write_value_type = Frame;
    using value_type = Frame;

    virtual ~WriteStream();
    virtual io::stream_result write(Frame& f, xec::notifiable* nobj) = 0;

    io::stream_result stream(Frame& f, xec::notifiable* nobj) final override {
        return write(f, nobj);
    }
};

} // namespace ac::frameio
