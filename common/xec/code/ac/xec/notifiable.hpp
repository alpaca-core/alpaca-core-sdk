// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"

namespace ac::xec {

class AC_XEC_API notifiable {
public:
    virtual ~notifiable();
    virtual void notify_all() = 0;
    virtual void notify_one() = 0;
};

} // namespace ac::xec