// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

// inline file
// needs a definition of DummyRegistry to be complete

#include "ac-test-data-dummy-models.h"

#include <ac/local/Model.hpp>
#include <ac/local/Instance.hpp>

#include <doctest/doctest.h>

#include <astl/move.hpp>

const ac::local::ModelDesc Model_Desc = {
    .inferenceType = "dummy",
    .assets = {
        {.path = AC_DUMMY_MODEL_SMALL}
    }
};

TEST_CASE("bad model") {
    DummyRegistry f;
    CHECK_THROWS_WITH(
        f.createModel({
            .inferenceType = "dummy",
            .assets = {
                {.path = "nope"}
            }
        }, {}),
        "Failed to open file: nope"
    );
}

TEST_CASE("bad instance") {
    DummyRegistry f;
    auto model = f.createModel(Model_Desc, {});
    REQUIRE(model);
    CHECK_THROWS_WITH(model->createInstance("nope", {}), "dummy: unknown instance type: nope");
    CHECK_THROWS_WITH(model->createInstance("general", { {"cutoff", 40} }),
        "Cutoff 40 greater than model size 3");
}

TEST_CASE("general") {
    DummyRegistry f;
    auto model = f.createModel(Model_Desc, {});
    REQUIRE(model);

    auto i = model->createInstance("general", {});
    REQUIRE(i);

    CHECK_THROWS_WITH(i->runOp("nope", {}), "dummy: unknown op: nope");

    CHECK_THROWS_WITH(i->runOp("run", { {"foo", "nope"} }), "Missing input");

    auto res = i->runOp("run", { {"input", {"a", "b"}} });
    CHECK(res.at("result").get<std::string>() == "a soco b bate");

    res = i->runOp("run", { {"input", {"a", "b"}}, {"splice", false} });
    CHECK(res.at("result").get<std::string>() == "a b soco bate vira");

    CHECK_THROWS_WITH(i->runOp("run", { {"input", {"a", "b"}}, {"throw_on", 3} }), "Throw on token 3");

    auto ci = model->createInstance("general", { {"cutoff", 2} });
    REQUIRE(ci);

    res = ci->runOp("run", { {"input", {"a", "b", "c"}} });
    CHECK(res.at("result").get<std::string>() == "a soco b bate c soco");
}

TEST_CASE("synthetic") {
    DummyRegistry f;

    std::string tag;
    float progress;
    auto model = f.createModel({
        .inferenceType = "dummy",
        .assets = {}
        }, {}, [&](const std::string_view t, float p) {
        tag = std::string(t);
        progress = p;
        return true;
    });
    REQUIRE(model);
    CHECK(tag == "synthetic");
    CHECK(progress == 0.5f);

    auto instance = model->createInstance("general", {});

    auto res = instance->runOp("run", { {"input", {"a", "b"}} });
    CHECK(res.at("result").get<std::string>() == "a one b two");
}
