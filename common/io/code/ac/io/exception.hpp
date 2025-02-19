// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include "status.hpp"
#include <stdexcept>

namespace ac::io {

struct AC_IO_API exception : public std::runtime_error {
    using std::runtime_error::runtime_error;
    ~exception();
};

struct AC_IO_API stream_closed_error : public exception {
    using exception::exception;
    ~stream_closed_error();

    static void throw_if_closed(const io::status& status) {
        if (status.closed()) {
            throw stream_closed_error("stream closed");
        }
    }

    static void throw_if_input_closed(const io::status& status) {
        if (status.closed()) {
            throw stream_closed_error("input stream closed");
        }
    }

    static void throw_if_output_closed(const io::status& status) {
        if (status.closed()) {
            throw stream_closed_error("output stream closed");
        }
    }
};

} // namespace ac::io
