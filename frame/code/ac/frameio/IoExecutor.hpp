// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../export.h"
#include <functional>
#include <memory>

namespace ac::frameio {

class AC_FRAME_EXPORT IoExecutor {
public:
    virtual ~IoExecutor();
    virtual void post(std::function<void()> task) = 0;
};

} // namespace ac::frameio
