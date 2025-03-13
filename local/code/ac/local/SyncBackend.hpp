// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Backend.hpp"
#include <ac/frameio/StreamEndpoint.hpp>
#include <ac/io/sync_io.hpp>

namespace ac::local {

class SyncBackend : private io::sync_io_ctx, private Backend {
public:
    SyncBackend() {
        m_xctx.system = &get_executor();
        m_xctx.io = &get_executor();
        m_xctx.dispatch = &get_executor();
        m_xctx.cpu = make_strand();
        m_xctx.gpu = make_strand();
    }

    using Io = io::sync_io<frameio::ReadStream, frameio::WriteStream>;

    Io connect(std::string_view serviceNameMatch) {
        auto ep = Backend::connect(serviceNameMatch, {1, 1});
        return Io(std::move(ep), *this);
    }
};

} // namespace ac::local
