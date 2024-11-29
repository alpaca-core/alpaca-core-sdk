// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/schema/Field.hpp>
#include <doctest/doctest.h>

using namespace ac::local::schema;

struct Person {
    Field<std::string> name;
    Field<int> age = std::nullopt;
    Field<bool> isChild = Default(false);
    Field<std::string> address = Default();
    Field<int> height = std::nullopt;
};

TEST_CASE("field") {
    Person p = {
        .name = "Alice"
    };
    CHECK(p.name == "Alice");
    CHECK_FALSE(p.name.defaultSet());

    CHECK_FALSE(p.age.hasValue());

    CHECK(p.address == "");
    CHECK(p.address.defaultSet());

    CHECK_FALSE(p.height.hasValue());

    p.age = 25;
    CHECK(p.age == 25);
    auto i = p.age.materialize();
    CHECK(i == 25);
    CHECK_FALSE(p.age.defaultSet());

    CHECK(p.isChild == false);
    CHECK(p.isChild.defaultSet());

    p.isChild = true;
    CHECK(p.isChild);
    CHECK_FALSE(p.isChild.defaultSet());

    i = p.height.materialize();
    CHECK(i == 0);
}
