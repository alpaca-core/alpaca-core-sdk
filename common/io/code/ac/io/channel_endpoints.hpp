// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "channel_stream.hpp"
#include "stream_endpoint.hpp"

namespace ac::io {

template <
    read_stream_class ReadStreamA,
    write_stream_class WriteStreamA,
    read_stream_class ReadStreamB = ReadStreamA,
    write_stream_class WriteStreamB = WriteStreamA
>
struct channel_endpoints {
    stream_endpoint<ReadStreamA, WriteStreamB> ab;
    stream_endpoint<ReadStreamB, WriteStreamA> ba;
};

template <
    read_stream_class ReadStreamA,
    write_stream_class WriteStreamA,
    read_stream_class ReadStreamB = ReadStreamA,
    write_stream_class WriteStreamB = WriteStreamA
>
channel_endpoints<ReadStreamA, WriteStreamB, ReadStreamB, WriteStreamA> make_channel_endpoints(
    channel_streams<ReadStreamA, WriteStreamA> as,
    channel_streams<ReadStreamB, WriteStreamB> bs
) {
    return {
        {std::move(as.out), std::move(bs.in)},
        {std::move(bs.out), std::move(as.in)}
    };
}

} // namespace ac::io
