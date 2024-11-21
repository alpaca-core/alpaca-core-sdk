// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/schema/TupleIndexByItemId.hpp>
#include <doctest/doctest.h>

using namespace ac::local::schema;

struct Zero {
    static inline constexpr std::string_view id = "zero";
};
struct One {
    static inline constexpr std::string_view id = "one";
};
struct Two {
    static inline constexpr std::string_view id = "two";
};

TEST_CASE("getTupleIndexByItemId") {
    using namespace ac::local::schema::impl;
    using Tuple = std::tuple<Zero, One, Two>;

    static_assert(getTupleIndexByItemId<Tuple>("zero") == 0);
    static_assert(getTupleIndexByItemId<Tuple>("one") == 1);
    static_assert(getTupleIndexByItemId<Tuple>("two") == 2);
    static_assert(getTupleIndexByItemId<Tuple>("fourty-two") == -1);

    std::string id = "zero";
    CHECK(getTupleIndexByItemId<Tuple>(id) == 0);
    id = "one";
    CHECK(getTupleIndexByItemId<Tuple>(id) == 1);
    id = "two";
    CHECK(getTupleIndexByItemId<Tuple>(id) == 2);
    id = "a million";
    CHECK(getTupleIndexByItemId<Tuple>(id) == -1);
}
