// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <cstdlib>

// test utility: setenv for Windows

#if defined(_WIN32)
// we can't use ::SetEnvironmentVariable as doesn't work with std::getenv :(

#include <vector>
#include <string_view>

namespace {
std::vector<std::vector<char>> vars;
void setenv(std::string_view name, std::string_view value, int) {
    std::vector<char> new_var(name.length() + value.length() + 1 /*=*/ + 1 /*0*/);
    char* str = new_var.data();
    memcpy(str, name.data(), name.length());
    str += name.length();
    *str++ = '=';
    memcpy(str, value.data(), value.length());
    str += value.length();
    *str = 0;

    auto& v = vars.emplace_back(std::move(new_var));
    _putenv(v.data());
}
}
#endif
