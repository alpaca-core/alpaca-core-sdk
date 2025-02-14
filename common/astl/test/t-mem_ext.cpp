// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <astl/mem_ext.hpp>
#include <doctest/doctest.h>

int c_unique_ptr_deleted = 0;

TEST_CASE("c_unique_ptr") {
    astl::c_unique_ptr<int> p(new int(42), [](int* p) {
        delete p;
        ++c_unique_ptr_deleted;
    });
    CHECK(*p == 42);
    p.reset(new int(43));
    CHECK(*p == 43);
    CHECK(c_unique_ptr_deleted == 1);
    p.reset();
    CHECK_FALSE(p);
    CHECK(c_unique_ptr_deleted == 2);
}