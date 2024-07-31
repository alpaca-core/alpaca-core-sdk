// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/Dict.hpp>
#include <doctest/doctest.h>
#include <astl/move.hpp>

// yes, nlohmann_json does have tests in its repo, but we still want to test our specific usage
// if we ever change the json library, we should use this to find regressions
// (it would likely need modifications but our use cases woule still be here)

TEST_CASE("empty") {
    ac::Dict d;
    CHECK(d.empty());
    CHECK(d.find("key") == d.end());
    CHECK(d.contains("key") == false);
}

TEST_CASE("from json") {
    CHECK_THROWS(std::ignore = ac::Dict::parse("not json"));

    std::string json = R"({
        "key": "value",
        "seven": 7,
        "obj": {
            "nested": "value"
        },
        "ar": [1, 2, "three"]
    })";

    auto d = ac::Dict::parse(json);
    CHECK(d.is_object());
    auto f = d.find("key");
    CHECK(f != d.end());
    CHECK(f->get<std::string>() == "value");

    // string views persist (as long as the objec is not touched)
    {
        std::string_view val = f->get<std::string_view>();
        CHECK(val == "value");
    }

    CHECK_THROWS(f->get<int>());

    CHECK(d.at("seven").get<int>() == 7);

    CHECK_THROWS(d.at("??"));

    f = d.find("nope");
    CHECK(f == d.end());

    f = d.find("obj");
    CHECK(f->is_object());

    f = d.find("ar");
    CHECK(f->is_array());
    CHECK(f->size() == 3);
    CHECK(f->at(0).get<int>() == 1);
    CHECK(f->at(2).get<std::string_view>() == "three");
    CHECK_THROWS(f->at(3));

    CHECK(ac::Dict_optValueAt(d, "key", std::string("default")) == "value");
    CHECK_THROWS(ac::Dict_optValueAt(d, "key", 7));
    CHECK(ac::Dict_optValueAt(d, "mey", std::string("default")) == "default");

    CHECK(ac::Dict_optValueAt(d, "seven", 6) == 7);
    CHECK(ac::Dict_optValueAt(d, "eight", 8) == 8);
}

TEST_CASE("to json") {
    ac::Dict d;
    d["key"] = "value";
    d["seven"] = 7;
    d["obj"] = ac::Dict{{"nested", "value"}};
    d["ar"] = ac::Dict::array({1, 2, "three"});

    CHECK(d.dump() == R"({"ar":[1,2,"three"],"key":"value","obj":{"nested":"value"},"seven":7})");
}

TEST_CASE("take") {
    std::string json = R"({
        "key": "value",
        "obj": {
            "nested": "very long value here in order to prevent small string optimizations"
        },
        "ar": [1, 2, "three"]
    })";
    auto d = ac::Dict::parse(json);

    std::string_view val;
    {
        auto f = d.find("obj");
        f = f->find("nested");
        val = f->get<std::string_view>();
        CHECK(val.starts_with("very long")); // sanity
    }

    auto mv = astl::move(d.at("obj"));
    CHECK(mv.is_object());
    CHECK(d.at("obj").is_null());

    auto f = mv.find("nested");
    CHECK(f->get<std::string_view>().data() == val.data()); // pointers preserved
}

TEST_CASE("binary") {
    ac::Blob blob = {1, 2, 4, 102, 0, 203};
    const void* data = blob.data();

    ac::Dict d;
    d["val"] = "some value";
    d["blob"] = ac::Dict::binary(std::move(blob));

    auto f = d.find("blob");
    CHECK(f->is_binary());

    auto& bin = f->get_binary();
    CHECK(bin.data() == data); // no copies
}
