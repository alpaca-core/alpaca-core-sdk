// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#pragma once
#include "API.h"

#include <string_view>
#include <memory>

namespace ac::jalog
{

class AsyncLogging;

class AC_JALOG_API AsyncLoggingThread {
    // implemented in AsyncLogging.cpp
public:
    AsyncLoggingThread(AsyncLogging& source, std::string_view threadName = "ac-jalog");
    ~AsyncLoggingThread();
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
