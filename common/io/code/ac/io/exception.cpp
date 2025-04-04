// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "exception.hpp"

namespace ac::io {
stream_closed_error::~stream_closed_error() = default;

const char* stream_closed_error::what() const _NOEXCEPT {
    switch (t) {
    case type::input:
        return "input stream closed";
    case type::output:
        return "output stream closed";
    default:
        return "stream closed";
    }
}

} // namespace ac::io
