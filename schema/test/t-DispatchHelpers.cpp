// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/schema/DispatchHelpers.hpp>
#include <ac/Dict.hpp>
#include <doctest/doctest.h>
#include <string>
#include <string_view>
#include <cstdlib>

struct Schema {
    struct ElemA {
        static constexpr auto id = "elem-a";
        using Params = int;
    };
    struct ElemB {
        static constexpr auto id = "elem-b";
        using Params = std::string;
    };
    struct ElemC {
        static constexpr auto id = "elem-c";
        using Params = double;
    };
    using Elems = std::tuple<ElemA, ElemB, ElemC>;
};

struct Dispatcher {
    std::string on(Schema::ElemA, int i) {
        return std::to_string(i);
    }
    std::string on(Schema::ElemB, const std::string& s) {
        return s + "!";
    }
    std::string on(Schema::ElemC, double d) {
        return on({}, int(d));
    }
    std::string on(std::string_view badId, ac::Dict&) {
        return on({}, std::string(badId));
    }
};

TEST_CASE("dispatchById") {
    Schema::Elems elems;
    Dispatcher disp;

    ac::Dict dict;
    CHECK(ac::local::schema::dispatchById<Schema::Elems>("elem-a", dict, disp) == "0");
    CHECK(ac::local::schema::dispatchById<Schema::Elems>("elem-b", dict, disp) == "!");
    CHECK(ac::local::schema::dispatchById<Schema::Elems>("elem-c", dict, disp) == "0");
    CHECK(ac::local::schema::dispatchById<Schema::Elems>("nope", dict, disp) == "nope!");
}