// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <astl/cow.hpp>
#include <vector>
#include <doctest/doctest.h>

struct test {
    test() = default;
    test(int x, float y) : x(x), y(y) {}
    int x = 7;
    float y = 3.5f;
    std::vector<int> vec;
};

TEST_CASE("basic") {
    astl::sp_cow<test> t;
    const void* vptr = t.detach().get();
    auto ptr = t.detach();
    REQUIRE(ptr);
    CHECK(vptr == ptr.get());

    CHECK(t->x == 7);
    CHECK(t->y == 3.5);
    CHECK(t->vec.empty());
    {
        auto& w = t.w();
        CHECK(w.x == 7);
        w.x = 4;
        CHECK(w.y == 3.5);
        w.vec.push_back(42);
        w.vec.push_back(53);
    }
    vptr = t.detach().get();
    auto ptr2 = t.detach();
    CHECK(ptr != ptr2);

    CHECK(t->x == 4);
    CHECK(ptr->x == 7);
    CHECK(t->vec.size() == 2);
    CHECK(ptr->vec.empty());

    CHECK(vptr == ptr2.get());

    {
        auto& w = t.w();
        vptr = &w;
        w.x = 5;
        CHECK(w.vec.size() == 2);
        w.vec.push_back(80);
    }

    CHECK(t->x == 5);
    CHECK(t->vec.size() == 3);
    CHECK(vptr == t.detach().get());
}

TEST_CASE("construct") {
    astl::sp_cow<test> t(42, 9.f);
    CHECK(t->x == 42);
    CHECK(t->y == 9.f);
}
