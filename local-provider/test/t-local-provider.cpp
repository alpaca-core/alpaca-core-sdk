// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "tl-dummy-local-provider.hpp"
#include <doctest/doctest.h>
#include <ac/LocalInference.hpp>
#include <ac/LocalProvider.hpp>

#include <ac/Model.hpp>
#include <ac/Instance.hpp>

#include <string>
#include <optional>
#include <latch>

struct AcTestHelper {
    ac::LocalProvider provider;
    ac::CallbackResult<ac::ModelPtr> modelResult;
    std::optional<std::latch> latch;
    float progress;
    DummyLocalInferenceModelLoader modelLoader;
    ac::CallbackResult<ac::InstancePtr> instanceResult;

    void createModelAndWait(const ac::Dict& args) {
        latch.emplace(1);

        provider.createModel(args, {
            [&](ac::CallbackResult<ac::ModelPtr> result) {
                modelResult = std::move(result);
                latch->count_down();
            },
            [&](float f) {
                progress = f;
            }
        });

        latch->wait();
    }

    void createInstanceAndWait(ac::Dict params) {
        latch.emplace(1);
        auto model = modelResult.value_or(ac::ModelPtr{});
        REQUIRE(model);
        model->createInstance("general", params, {
            [&](ac::CallbackResult<ac::InstancePtr> result) {
                instanceResult = std::move(result);
                latch->count_down();
            },
            {} // no progress callback
        });
        latch->wait();
    }
};

TEST_CASE("invalid args") {
    AcTestHelper h;
    h.createModelAndWait({{}});

    REQUIRE(h.modelResult.has_error() == true);
    CHECK(h.modelResult.error().text == "[json.exception.type_error.304] cannot use at() with array");
}

TEST_CASE("missing model provider") {
    AcTestHelper h;
    h.createModelAndWait({{"type", "llama.cpp"}, {"error", true}});

    REQUIRE(h.modelResult.has_error() == true);
    CHECK(h.modelResult.error().text == "Unknown model type");
}

TEST_CASE("model loading error") {
    AcTestHelper h;
    h.provider.addLocalInferenceLoader("llama.cpp", h.modelLoader);
    h.createModelAndWait({{"type", "llama.cpp"}, {"error", true}});

    REQUIRE(h.modelResult.has_error() == true);
    CHECK(h.modelResult.error().text == "Model couldn't be loaded!");
    CHECK(h.progress == 0.5f);
}

TEST_CASE("model loading success") {
    AcTestHelper h;
    h.provider.addLocalInferenceLoader("llama.cpp", h.modelLoader);
    h.createModelAndWait({{"type", "llama.cpp"}});

    REQUIRE(h.modelResult.has_value() == true);
    CHECK(h.modelResult.value());
    CHECK(h.progress == 1.f);
}

TEST_CASE("instance loading error") {
    AcTestHelper h;
    h.provider.addLocalInferenceLoader("llama.cpp", h.modelLoader);
    h.createModelAndWait({{"type", "llama.cpp"}});
    h.createInstanceAndWait({{"error", true}});

    REQUIRE(h.instanceResult.has_error() == true);
    CHECK(h.instanceResult.error().text == "Instance couldn't be created!");
}

TEST_CASE("instance loading success") {
    AcTestHelper h;
    h.provider.addLocalInferenceLoader("llama.cpp", h.modelLoader);
    h.createModelAndWait({{"type", "llama.cpp"}});
    h.createInstanceAndWait({{}});

    REQUIRE(h.instanceResult.has_value() == true);
    CHECK(h.instanceResult.value());
}

TEST_CASE("run ops") {
    AcTestHelper h;
    h.provider.addLocalInferenceLoader("llama.cpp", h.modelLoader);
    h.createModelAndWait({{"type", "llama.cpp"}});
    h.createInstanceAndWait({{}});

    REQUIRE(h.instanceResult.has_value() == true);
    CHECK(h.instanceResult.value());

    auto instance = h.instanceResult.value();
    std::string opError;
    ac::Dict opResult;
    unsigned resultsCount = 0;

    instance->runOp("insta", {{}}, {
        [&](ac::CallbackResult<void> result) {
            if (result.has_error()) {
                opError = std::move(result.error().text);
                return;
            }
        },
        [&](ac::Dict result) {
            opResult[resultsCount++] = result;
        }
    });
    instance->synchronize();

    CHECK(resultsCount == 1);
    CHECK(opResult[0]["insta"] == "success");

    resultsCount = 0;
    opResult = {};

    instance->runOp("more", {{}}, {
        [&](ac::CallbackResult<void> result) {
            if (result.has_error()) {
                opError = std::move(result.error().text);
                return;
            }
        },
        [&](ac::Dict result) {
            opResult[resultsCount++] = result;
        }
    });
    instance->synchronize();

    CHECK(resultsCount == 2);
    CHECK(opResult[0]["some"] == 42);
    CHECK(opResult[1]["more"] == 1024);
}


