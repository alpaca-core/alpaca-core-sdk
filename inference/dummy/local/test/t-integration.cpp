// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ac-test-data-dummy-models.h"

#include <ac/LocalDummy.hpp>

#include <ac/LocalInference.hpp>
#include <ac/LocalProvider.hpp>
#include <ac/ModelInfo.hpp>
#include <ac/AssetSourceLocalDir.hpp>

#include <ac/Model.hpp>
#include <ac/Instance.hpp>

#include <doctest/doctest.h>

#include <astl/move.hpp>
#include <latch>

struct TestHelper {
    ac::LocalProvider provider;

    std::optional<std::latch> latch;

    TestHelper() {
        ac::addLocalDummyInference(provider);
        provider.addAssetSource(ac::AssetSourceLocalDir_Create(AC_TEST_DATA_DUMMY_DIR), 0);

        provider.addModel(ac::ModelInfo{
            .id = "dummy-small",
            .inferenceType = "dummy",
            .assets = {
                {AC_DUMMY_MODEL_SMALL_ASSET, "x"},
            }
        });
    }

    ac::ModelPtr model;
    void createModel(std::string_view id, ac::Dict params) {
        latch.emplace(1);
        provider.createModel(id, params, {
            [&](ac::CallbackResult<ac::ModelPtr> result) {
                model = result.value_or(nullptr);
                latch->count_down();
            },
            {}
        });
        latch->wait();

    }

    ac::InstancePtr instance;
    void createInstance(std::string_view type, ac::Dict params) {
        latch.emplace(1);
        model->createInstance(type, astl::move(params), {
            [&](ac::CallbackResult<ac::InstancePtr> result) {
                instance = result.value_or(nullptr);
                latch->count_down();
            },
            {}
        });
        latch->wait();
    }

    ac::CallbackResult<std::vector<ac::Dict>> runOp(std::string_view op, ac::Dict params) {
        ac::CallbackResult<std::vector<ac::Dict>> ret;
        latch.emplace(1);
        instance->runOp(op, astl::move(params), {
            [&](ac::CallbackResult<void> result) {
                if (result.has_error()) {
                    ret = itlib::unexpected(result.error());
                }
                latch->count_down();
            },
            [&](ac::Dict res) {
                REQUIRE(ret.has_value());
                ret->push_back(astl::move(res));
            }
        });
        latch->wait();
        return ret;
    }

    ac::CallbackResult<ac::Dict> runInstantOp(std::string_view op, ac::Dict params) {
        auto res = runOp(op, params);
        ac::CallbackResult<ac::Dict> ret;
        if (res.has_error()) {
            ret = itlib::unexpected(res.error());
        }
        else {
            REQUIRE(res->size() == 1);
            ret = std::move(res.value().front());
        }
        return ret;
    }
};

TEST_CASE("bad model") {
    TestHelper helper;
    helper.createModel("nope", {});
    CHECK_FALSE(helper.model);
}

TEST_CASE("bad instance") {
    TestHelper helper;
    helper.createModel("dummy-small", {});
    REQUIRE(helper.model);
    helper.createInstance("nope", {});
    CHECK_FALSE(helper.instance);
    helper.createInstance("general", {{"cutoff", 40}});
    CHECK_FALSE(helper.instance);
}

TEST_CASE("general") {
    TestHelper helper;
    helper.createModel("dummy-small", {});
    REQUIRE(helper.model);

    helper.createInstance("general", {});
    REQUIRE(helper.instance);

    auto res = helper.runInstantOp("nope", {});
    REQUIRE(res.has_error());
    CHECK(res.error().text == "dummy: unknown op: nope");

    res = helper.runInstantOp("run", {{"foo", "nope"}});
    REQUIRE(res.has_error());
    CHECK(res.error().text == "[json.exception.out_of_range.403] key 'input' not found");

    res = helper.runInstantOp("run", {{"input", {"a", "b"}}});
    REQUIRE(res.has_value());
    CHECK(res.value().at("result").get<std::string>() == "a soco b bate");

    res = helper.runInstantOp("run", {{"input", {"a", "b"}}, {"splice", false}});
    REQUIRE(res.has_value());
    CHECK(res.value().at("result").get<std::string>() == "a b soco bate vira");

    res = helper.runInstantOp("run", {{"input", {"a", "b"}}, {"throw_on", 3}});
    REQUIRE(res.has_error());
    CHECK(res.error().text == "Throw on token 3");

    helper.createInstance("general", {{"cutoff", 2}});
    REQUIRE(helper.instance);

    res = helper.runInstantOp("run", {{"input", {"a", "b", "c"}}});
    REQUIRE(res.has_value());
    CHECK(res.value().at("result").get<std::string>() == "a soco b bate c soco");
}

