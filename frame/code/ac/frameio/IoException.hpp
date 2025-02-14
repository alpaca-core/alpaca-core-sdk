// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../export.h"
#include <ac/io/status.hpp>
#include <stdexcept>

namespace ac::frameio {

struct AC_FRAME_EXPORT IoException : public std::runtime_error {
    using std::runtime_error::runtime_error;
    ~IoException();
};

struct AC_FRAME_EXPORT IoClosed : public IoException {
    using IoException::IoException;
    ~IoClosed();

    static void throwInputIfClosed(const io::status& status) {
        if (status.closed()) {
            throw IoClosed("input closed");
        }
    }
    static void throwOutputIfClosed(const io::status& status) {
        if (status.closed()) {
            throw IoClosed("output closed");
        }
    }
};

} // namespace ac::frameio
