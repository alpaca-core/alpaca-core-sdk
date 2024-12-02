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
    int on(Schema::ElemA, int i) {
        return i + 5;
    }
    std::string on(Schema::ElemB, const std::string& s) {
        return s + "!";
    }
    int on(Schema::ElemC, double d) {
        return int(d);
    }
    std::string on(std::string_view badId, ac::Dict) {
        return on({}, std::string(badId));
    }
};

TEST_CASE("dispatchById") {
    Schema::Elems elems;
    Dispatcher disp;

    //CHECK(ac::local::schema::dispatchById<Schema::Elems>("elem-a", ac::Dict(5), disp) == "5");
    //CHECK(ac::local::schema::dispatchById<Schema::Elems>("elem-b", ac::Dict("hi"), disp) == "hi!");
    //CHECK(ac::local::schema::dispatchById<Schema::Elems>("elem-c", ac::Dict(3.14), disp) == "3");
    //CHECK(ac::local::schema::dispatchById<Schema::Elems>("nope", ac::Dict(nullptr), disp) == "nope!");
}