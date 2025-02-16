// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../../export.h"
#include "../SessionHandlerPtr.hpp"
#include <ac/xec/context.hpp>

namespace ac::frameio {

struct StreamEndpoint;

class AC_FRAME_EXPORT LocalIoCtx : public xec::context {
public:
    void connect(SessionHandlerPtr handler, StreamEndpoint ep);
};

} // namespace ac::frameio
