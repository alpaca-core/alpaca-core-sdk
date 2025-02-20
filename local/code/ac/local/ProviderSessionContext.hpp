// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/xec/strand.hpp>
#include <ac/frameio/StreamEndpoint.hpp>

namespace ac::local {

struct ProviderSessionContext {
    struct Executors {
        xec::strand dispatch;
        xec::strand cpu;
        xec::strand gpu;
        // intentionally leaving system and io outside of the context
        // it is not the session's job to deal with those
    } executor;

    struct Endpoints {
        frameio::StreamEndpoint session;
        frameio::StreamEndpoint system;
    } endpoint;
};

} // namespace ac::local
