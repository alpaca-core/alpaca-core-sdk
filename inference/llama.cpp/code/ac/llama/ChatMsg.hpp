// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <string>

namespace ac::llama {

struct ChatMsg {
    std::string role; // who sent the message
    std::string text; // the message's content
};

} // namespace ac::llama

