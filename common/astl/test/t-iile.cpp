// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <astl/iile.h>
#include <doctest/doctest.h>

TEST_CASE("iile") {
    CHECK(iile([] {return 5; }) == 5);
}