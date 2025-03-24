// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <astl/id_ptr.hpp>
#include <doctest/doctest.h>

TEST_CASE("id_ptr") {
    void* null = nullptr;
    void* bf = reinterpret_cast<void*>(0xbaadf00dull);
    void* dbfc = reinterpret_cast<void*>(0xdeadbeeffeedc0deull);

    CHECK(astl::id_from_ptr(null) == "0x0");
    CHECK(astl::id_from_ptr(bf) == "0xbaadf00d");
    CHECK(astl::id_from_ptr(dbfc, "ptr:") == "ptr:deadbeeffeedc0de");

    CHECK(astl::id_or_ptr("", bf, "p") == "pbaadf00d");
    CHECK(astl::id_or_ptr("id", dbfc, "p") == "id");
}
