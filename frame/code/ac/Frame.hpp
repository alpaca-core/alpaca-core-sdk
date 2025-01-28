// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Dict.hpp"

namespace ac {

struct Frame {
    std::string op;
    Dict data;

    static Frame justOp(std::string op) {
        return {std::move(op), {}};
    }
};

} // namespace ac
