// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <doctest/doctest.h>

#include <ac/local/ResourceManager.hpp>

#include <string>

struct StrResource : public std::string, public ac::local::Resource {
    using std::string::basic_string;
};

TEST_CASE("basic") {
    ac::local::ResourceManager<std::string, StrResource> rm;
    {
        // empty
        auto res = rm.find("empty");
        CHECK(!res);
    }

    auto orig = rm.add("orig", std::make_shared<StrResource>("orig"));
    CHECK(*orig == "orig");

    {
        // not empty
        auto added = rm.add("some-key", std::make_shared<StrResource>("value"));
        CHECK(added != orig);
        added->maxAge = ac::local::Resource::seconds_t::max();

        auto found = rm.find("some-key");
        CHECK(found);
        CHECK(added == found);
        CHECK(added.get() == found.get());
        CHECK(*added == "value");
    }

    {
        auto res = rm.findOrCreate("some-key", [](const std::string&) {
            return std::make_shared<StrResource>("value2");
        });
        CHECK(*res == "value");

        auto found = rm.find("some-key");
        CHECK(res == found);
    }

    {
        auto res = rm.findOrCreate("some-other-key", [](const std::string&) {
            return std::make_shared<StrResource>("value2");
        });

        CHECK(*res == "value2");

        res->maxAge = std::chrono::seconds(0);
    }

    // Collect resources that expired
    rm.garbageCollect();

    CHECK(rm.find("some-key"));
    CHECK_FALSE(rm.find("some-other-key"));

    {
        // test that if we hold the resource, it won't be collected
        auto res = rm.findOrCreate("some-other-key", [](const std::string&) {
            return std::make_shared<StrResource>("value3");
        });

        CHECK(*res == "value3");

        res->maxAge = std::chrono::seconds(0);

        rm.garbageCollect();

        CHECK(*res == "value3");
        CHECK(res == rm.find("some-other-key"));
    }

    // Force to garbage collect all resources
    rm.garbageCollect(true);

    // Check that all resources are collected, but the one we hold is not
    CHECK(rm.find("orig"));
    CHECK_FALSE(rm.find("some-key"));
}
