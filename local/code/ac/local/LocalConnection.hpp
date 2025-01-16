// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <ac/SessionHandlerPtr.hpp>

namespace ac::local {

class AC_LOCAL_EXPORT LocalConnection {
public:
    LocalConnection();
    LocalConnection(const LocalConnection&) = delete;
    LocalConnection& operator=(const LocalConnection&) = delete;
    ~LocalConnection();

    void run();

    void connect(SessionHandlerPtr a, SessionHandlerPtr b);

    void stop();
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace ac::local
