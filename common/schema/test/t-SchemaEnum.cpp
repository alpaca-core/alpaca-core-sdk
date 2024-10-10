// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/schema/SchemaEnum.hpp>
#include <doctest/doctest.h>

using namespace ac::schema;

enum Week {
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday
};

TEST_CASE("get") {
    auto dict = Dict::parse("\"Tuesday\"");
    const Enum<Week> e(dict);
    CHECK(e.getValue() == Week::Tuesday);

    dict = Dict::parse("\"Nope\"");
    const Enum<Week> e2(dict);
    CHECK_THROWS_WITH(e2.getValue(), "Invalid enum value");
}

TEST_CASE("set") {
    Dict d;
    Enum<Week> e(d);

    e.setValue(Week::Wednesday);
    CHECK(d == "Wednesday");

    e.setValue("Friday");
    CHECK(d == "Friday");

    CHECK_THROWS_WITH(e.setValue("Invalid"), "Invalid enum value");
}

TEST_CASE("desc") {
    const Enum<Week> e;
    OrderedDict d;
    e.describeSelf(d);

    std::string_view expected = R"({
    "type": "string",
    "enum": [
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday",
        "Sunday"
    ]
})";
    CHECK(d.dump(4) == expected);
}