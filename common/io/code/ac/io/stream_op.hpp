// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "concepts/stream.hpp"

namespace ac::io {

template <write_stream_class Stream, on_blocked_cb_class OnBlocked>
status stream_op(Stream& str, typename Stream::write_value_type& t, OnBlocked&& on_blocked) {
    return str.write(t, std::forward<OnBlocked>(on_blocked));
}

template <read_stream_class Stream, on_blocked_cb_class OnBlocked>
status stream_op(Stream& str, typename Stream::read_value_type& t, OnBlocked&& on_blocked) {
    return str.read(t, std::forward<OnBlocked>(on_blocked));
}

} // namespace ac::io
