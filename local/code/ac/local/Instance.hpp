// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "ProgressCb.hpp"
#include <ac/Dict.hpp>
#include <string_view>

namespace ac::local {
class AC_LOCAL_EXPORT Instance {
public:
    virtual ~Instance();

    virtual Dict runOp(std::string_view op, Dict params, ProgressCb cb = {}) = 0;
};
} // namespace ac::local
