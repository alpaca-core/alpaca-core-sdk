// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include "Frame.hpp"
#include <ac/io/stream_result.hpp>

namespace ac::rpc {

class AC_RPC_API Stream {
public:
    using OnBlockedFunc = astl::ufunction<xec::task()>;

    virtual ~Stream();
    virtual io::stream_result stream(Frame& f, OnBlockedFunc onBlocked) = 0;
    virtual void close() = 0;
};

class AC_RPC_API ReadStream : public Stream {
public:
    using read_value_type = Frame;
    using value_type = Frame;

    virtual ~ReadStream();
    virtual io::stream_result read(Frame& f, OnBlockedFunc onBlocked) = 0;

    io::stream_result stream(Frame& f, OnBlockedFunc onBlocked) final override {
        return read(f, std::move(onBlocked));
    }
};

class AC_RPC_API WriteStream : public Stream {
public:
    using write_value_type = Frame;
    using value_type = Frame;

    virtual ~WriteStream();
    virtual io::stream_result write(Frame& f, OnBlockedFunc onBlocked) = 0;

    io::stream_result stream(Frame& f, OnBlockedFunc onBlocked) final override {
        return write(f, std::move(onBlocked));
    }
};

} // namespace ac::rpc
