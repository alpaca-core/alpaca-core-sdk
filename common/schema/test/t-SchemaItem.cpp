// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/schema/SchemaItem.hpp>
#include <doctest/doctest.h>

using namespace ac::schema;

TEST_CASE("trivial null") {
    Int i;
    CHECK_FALSE(i.getValue());
}

TEST_CASE("trivial val") {
    Dict d;
    Uint u(d);
    CHECK_FALSE(u.getValue());

    u.setValue(42);
    CHECK(u.getValue() == 42);
}

struct Human : public Object {
    using Object::Object;
    String name{*this, "name", "The name of the human"};
    Int age{*this, "age", "The age of the human", {}, true};
    Double weight{*this, "weight", "The weight of the human", 0};
    Bool member{*this, "member", "Is the human member of the org" };
};

TEST_CASE("simple get") {
    auto dict = Dict::parse(R"({
        "name": "John",
        "age": 42
    })");
    const Human h(dict);
    CHECK(h.name.getValue() == "John");
    CHECK(h.age.getValue() == 42);
    CHECK(h.weight.getValue() == 0);
    CHECK(h.member.getValue() == std::nullopt);
}

TEST_CASE("simple set") {
    Dict d;
    Human h(d);

    h.name.setValue("Alice");
    h.age.setValue(36);

    CHECK(d["name"] == "Alice");
    CHECK(d["age"] == 36);

    auto f = d.find("weight");
    CHECK(f == d.end());
    f = d.find("member");
    CHECK(f == d.end());

    h.weight.setValue(55.5);
    h.member.setValue(false);

    CHECK(d["weight"] == 55.5);
    CHECK(d["member"] == false);
}

TEST_CASE("simple desc") {
    const Human h;
    OrderedDict d;
    h.describeSelf(d);

    std::string_view expected = R"({
    "type": "object",
    "properties": {
        "name": {
            "description": "The name of the human",
            "type": "string"
        },
        "age": {
            "description": "The age of the human",
            "type": "integer"
        },
        "weight": {
            "description": "The weight of the human",
            "type": "number",
            "default": 0
        },
        "member": {
            "description": "Is the human member of the org",
            "type": "boolean"
        }
    },
    "required": [
        "age"
    ]
})";

    CHECK(d.dump(4) == expected);
}

struct Location : public Object {
    using Object::Object;
    String city{*this, "city"};
    String country{ *this, "country"};
};

struct Org : public Object {
    using Object::Object;
    String name{*this, "name", "The name of the org"};
    Location location{*this, "location"};
    Array<Human> members{*this, "members", "The members of the org"};
};

// note the alphabetical order of keys (as per nlohmann::json)
constexpr std::string_view Org_Json = R"({
    "location": {
        "city": "Pernik",
        "country": "Bulgaria"
    },
    "members": [
        {
            "age": 35,
            "name": "Alice"
        },
        {
            "age": 42,
            "member": true,
            "name": "Bob",
            "weight": 88.0
        }
    ],
    "name": "AC"
})";

TEST_CASE("complex get") {
    Dict dict = Dict::parse(Org_Json);
    const Org o(dict);
    CHECK(o.name.getValue() == "AC");
    CHECK(o.location.city.getValue() == "Pernik");
    CHECK(o.location.country.getValue() == "Bulgaria");

    auto& members = o.members;
    CHECK(members.size() == 2);
    auto alice = members[0];
    CHECK(alice.name.getValue() == "Alice");
    CHECK(alice.age.getValue() == 35);
    CHECK(alice.weight.getValue() == 0);
    CHECK(alice.member.getValue() == std::nullopt);
    auto bob = members[1];
    CHECK(bob.name.getValue() == "Bob");
    CHECK(bob.age.getValue() == 42);
    CHECK(bob.weight.getValue() == 88);
    CHECK(bob.member.getValue() == true);

    CHECK_THROWS_WITH(members[2], "[json.exception.out_of_range.401] array index 2 is out of range");
}

TEST_CASE("complex set") {
    Dict d;
    Org o(d);

    o.name.setValue("AC");
    o.location.city.setValue("Pernik");
    o.location.country.setValue("Bulgaria");

    auto& members = o.members;
    auto alice = members.emplace_back();
    alice.name.setValue("Alice");
    alice.age.setValue(35);

    auto bob = members.emplace_back();
    bob.name.setValue("Bob");
    bob.age.setValue(42);
    bob.weight.setValue(88);
    bob.member.setValue(true);

    CHECK(d.dump(4) == Org_Json);
}

TEST_CASE("complex desc") {
    const Org o;
    OrderedDict d;
    o.describeSelf(d);

    std::string_view expected = R"({
    "type": "object",
    "properties": {
        "name": {
            "description": "The name of the org",
            "type": "string"
        },
        "location": {
            "type": "object",
            "properties": {
                "city": {
                    "type": "string"
                },
                "country": {
                    "type": "string"
                }
            }
        },
        "members": {
            "description": "The members of the org",
            "type": "array",
            "items": {
                "type": "object",
                "properties": {
                    "name": {
                        "description": "The name of the human",
                        "type": "string"
                    },
                    "age": {
                        "description": "The age of the human",
                        "type": "integer"
                    },
                    "weight": {
                        "description": "The weight of the human",
                        "type": "number",
                        "default": 0
                    },
                    "member": {
                        "description": "Is the human member of the org",
                        "type": "boolean"
                    }
                },
                "required": [
                    "age"
                ]
            }
        }
    }
})";
    CHECK(d.dump(4) == expected);
}
