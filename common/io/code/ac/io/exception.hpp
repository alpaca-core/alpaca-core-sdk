// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include "status.hpp"
#include <exception>

namespace ac::io {

struct AC_IO_API stream_closed_error : public std::exception {
    enum class type {
        unknown,
        input,
        output,
    };
    type t;

    stream_closed_error(type t) : t(t) {}
    ~stream_closed_error();

    virtual const char* what() const override;

    static void throw_if_closed(const io::status& status) {
        if (status.closed()) {
            throw stream_closed_error(type::unknown);
        }
    }

    static void throw_if_input_closed(const io::status& status) {
        if (status.closed()) {
            throw stream_closed_error(type::input);
        }
    }

    static void throw_if_output_closed(const io::status& status) {
        if (status.closed()) {
            throw stream_closed_error(type::output);
        }
    }
};

} // namespace ac::io
