// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../StreamEndpoint.hpp"
#include "LocalChannel.hpp"

namespace ac::frameio {

struct LocalStreams {
    ReadStreamPtr in;
    WriteStreamPtr out;
};

inline LocalStreams LocalChannel_getStreams(const LocalChannelPtr& channel) {
    LocalStreams streams;
    streams.in = std::make_unique<LocalReadStream>(channel);
    streams.out = std::make_unique<LocalWriteStream>(channel);
    return streams;
}

struct LocalEndpoints {
    StreamEndpoint ab;
    StreamEndpoint ba;
};

inline LocalEndpoints LocalChannel_getEndpoints(const LocalChannelPtr& a, const LocalChannelPtr& b) {
    auto as = LocalChannel_getStreams(a);
    auto bs = LocalChannel_getStreams(b);

    LocalEndpoints ret;
    ret.ab.readStream = std::move(as.in);
    ret.ab.writeStream = std::move(bs.out);
    StreamEndpoint remote;
    ret.ba.readStream = std::move(bs.in);
    ret.ba.writeStream = std::move(as.out);
    return ret;
}

} // namespace ac::frameio
