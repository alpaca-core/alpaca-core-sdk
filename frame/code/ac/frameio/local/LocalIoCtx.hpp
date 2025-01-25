// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../../export.h"
#include "../SessionHandlerPtr.hpp"

namespace ac::frameio {

struct StreamEndpoint;

class AC_FRAME_EXPORT LocalIoCtx {
public:
    LocalIoCtx();
    LocalIoCtx(const LocalIoCtx&) = delete;
    LocalIoCtx& operator=(const LocalIoCtx&) = delete;
    ~LocalIoCtx();

    void run();
    void forceStop();
    void complete();

    void connect(SessionHandlerPtr handler, StreamEndpoint ep);
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace ac::frameio
