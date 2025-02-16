// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "concepts/stream.hpp"

namespace ac::io {

template <write_stream_class Stream>
status stream_op(Stream& str, typename Stream::write_value_type& t, xec::notifiable* nobj) {
    return str.write(t, nobj);
}

template <read_stream_class Stream>
status stream_op(Stream& str, typename Stream::read_value_type& t, xec::notifiable* nobj) {
    return str.read(t, nobj);
}

} // namespace ac::io
