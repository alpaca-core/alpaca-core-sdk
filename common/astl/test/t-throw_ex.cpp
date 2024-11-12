// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <astl/throw_stdex.hpp>
#include <doctest/doctest.h>

[[noreturn]] void throw_val(int val) {
    ac::throw_ex{} << "val=" << val;
}

TEST_CASE("c_unique_ptr") {
    CHECK_THROWS_WITH_AS(throw_val(42), "val=42", std::runtime_error);
}
