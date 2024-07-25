// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <astl/u8c.h>
#include <doctest/doctest.h>
#include <string>

TEST_CASE("c_unique_ptr") {
    std::string s = U8C("Hello");
    s += ", World!";
    CHECK(s == "Hello, World!");

    s = U8C("ハロー");
    s += U8C("、ワールド！");
    CHECK(s == U8C("ハロー、ワールド！"));
}
