// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <xec/strand.hpp>
#include <ac/frameio/StreamEndpoint.hpp>

namespace ac::local {

struct SessionContext {
    struct Executors {
        xec::strand dispatch;
        xec::strand cpu;
        xec::strand gpu;
        // intentionally leaving system and io outside of the context
        // it is not the session's job to deal with those
    } executors;

    struct Endpoints {
        frameio::StreamEndpoint session;
        frameio::StreamEndpoint system;
    } endpoints;
};

} // namespace ac::local
