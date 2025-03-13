// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Backend.hpp"

namespace ac::local {

class AC_LOCAL_EXPORT DefaultBackend : public Backend {
public:
    DefaultBackend();
    DefaultBackend(const DefaultBackend&) = delete;
    DefaultBackend& operator=(const DefaultBackend&) = delete;
    ~DefaultBackend();

    void join(bool forceStop = false);
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace ac::local
