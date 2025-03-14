// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Backend.hpp"

namespace ac::local {

namespace impl {
struct AC_LOCAL_EXPORT DefaultBackendXctx {
    DefaultBackendXctx();
    DefaultBackendXctx(const DefaultBackendXctx&) = delete;
    DefaultBackendXctx& operator=(const DefaultBackendXctx&) = delete;
    ~DefaultBackendXctx();

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
}

class AC_LOCAL_EXPORT DefaultBackend : private impl::DefaultBackendXctx, public Backend {
public:
    explicit DefaultBackend(std::string_view name = {});
    DefaultBackend(const DefaultBackend&) = delete;
    DefaultBackend& operator=(const DefaultBackend&) = delete;
};

} // namespace ac::local
