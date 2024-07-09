// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <cstdint>
#include <string>

namespace ac::api {
struct Error {
    int32_t code;
    std::string text;
};
}