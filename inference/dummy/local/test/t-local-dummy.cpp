// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ac-test-data-dummy-models.h"

#include <ac/local/LocalDummy.hpp>

#include <ac/local/ModelFactory.hpp>

#include <ac/local/Model.hpp>
#include <ac/local/Instance.hpp>

#include <doctest/doctest.h>

#include <astl/move.hpp>

struct DummyFactory : public ac::local::ModelFactory {
    DummyFactory() {
        ac::local::addDummyInference(*this);
    }
};

const ac::local::ModelDesc Model_Desc = {
    .inferenceType = "dummy",
    .assets = {
        {.path = AC_DUMMY_MODEL_SMALL}
    }
};

TEST_CASE("bad model") {
    DummyFactory f;
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
    DummyFactory f;
    auto model = f.createModel(Model_Desc, {});
    REQUIRE(model);
    CHECK_THROWS_WITH(model->createInstance("nope", {}), "dummy: unknown instance type: nope");
    CHECK_THROWS_WITH(model->createInstance("general", {{"cutoff", 40}}),
        "Cutoff 40 greater than model size 3");
}

TEST_CASE("general") {
    DummyFactory f;
    auto model = f.createModel(Model_Desc, {});
    REQUIRE(model);

    auto i = model->createInstance("general", {});
    REQUIRE(i);

    CHECK_THROWS_WITH(i->runOp("nope", {}), "dummy: unknown op: nope");

    CHECK_THROWS_WITH(i->runOp("run", {{"foo", "nope"}}),
        "[json.exception.out_of_range.403] key 'input' not found");

    auto res = i->runOp("run", {{"input", {"a", "b"}}});
    CHECK(res.at("result").get<std::string>() == "a soco b bate");

    res = i->runOp("run", {{"input", {"a", "b"}}, {"splice", false}});
    CHECK(res.at("result").get<std::string>() == "a b soco bate vira");

    CHECK_THROWS_WITH(i->runOp("run", {{"input", {"a", "b"}}, {"throw_on", 3}}), "Throw on token 3");

    auto ci = model->createInstance("general", {{"cutoff", 2}});
    REQUIRE(ci);

    res = ci->runOp("run", {{"input", {"a", "b", "c"}}});
    CHECK(res.at("result").get<std::string>() == "a soco b bate c soco");
}

TEST_CASE("synthetic") {
    DummyFactory f;

    auto model = f.createModel({
        .inferenceType = "dummy",
        .assets = {}
    }, {});
    REQUIRE(model);

    auto instance = model->createInstance("general", {});

    auto res = instance->runOp("run", {{"input", {"a", "b"}}});
    CHECK(res.at("result").get<std::string>() == "a one b two");
}
