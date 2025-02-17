// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/io/xio_endpoint.hpp>
#include <ac/io/buffered_channel.hpp>
#include <ac/io/channel_stream.hpp>
#include <ac/io/channel_endpoints.hpp>

#include <ac/xec/timer_wobj.hpp>
#include <ac/xec/strand_wobj.hpp>

#include <doctest/doctest.h>

#include <string>

template <typename Frame>
struct t_io {
    using channel = ac::io::buffered_channel<Frame>;
    using read_stream = ac::io::channel_read_stream<channel>;
    using write_stream = ac::io::channel_write_stream<channel>;
    using t_ep = ac::io::xio_endpoint<read_stream, write_stream, ac::xec::timer_wobj>;
    using s_ep = ac::io::xio_endpoint<read_stream, write_stream, ac::xec::strand_wobj>;
};

using int_io = t_io<int>;
using string_io = t_io<std::string>;

