// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <astl/move.hpp>

#include <string>

int main() {
    const std::string cstr = "asdf";
    // build error: cannot move a const object
    auto fail = astl::move(cstr);
}
