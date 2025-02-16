// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalIoCtx.hpp"
#include "../SessionHandler.hpp"
#include "../StreamEndpoint.hpp"
#include "../IoExecutor.hpp"

namespace ac::frameio {

void LocalIoCtx::connect(SessionHandlerPtr handler, StreamEndpoint ep) {
    auto strand = make_strand();
    IoExecutor executor{strand};
    SessionHandler::init(
        handler,
        executor.attachInput(std::move(ep.read_stream)),
        executor.attachOutput(std::move(ep.write_stream)),
        executor
    );
}

} // namespace ac::frameio
