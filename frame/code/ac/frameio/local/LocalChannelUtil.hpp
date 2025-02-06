// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../StreamEndpoint.hpp"
#include "LocalChannel.hpp"

namespace ac::frameio {

struct LocalEndpoints {
    StreamEndpoint ab;
    StreamEndpoint ba;
};

inline LocalEndpoints LocalChannel_getEndpoints(LocalChannelPtr a, LocalChannelPtr b) {
    auto as = LocalChannel_getStreams(std::move(a));
    auto bs = LocalChannel_getStreams(std::move(b));

    LocalEndpoints ret;
    ret.ab.readStream = std::move(as.in);
    ret.ab.writeStream = std::move(bs.out);
    StreamEndpoint remote;
    ret.ba.readStream = std::move(bs.in);
    ret.ba.writeStream = std::move(as.out);
    return ret;
}

} // namespace ac::frameio
