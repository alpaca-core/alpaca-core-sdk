// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <string>

namespace ac::rpc {

struct Frame {
    std::string op;
    std::string data;

    static Frame justOp(std::string op) {
        return { std::move(op), {} };
    }
};

} // namespace ac::rpc
