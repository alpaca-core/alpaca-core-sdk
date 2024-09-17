// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "tl-dummy-local-provider.hpp"
#include <doctest/doctest.h>
#include <ac/LocalInference.hpp>
#include <ac/LocalProvider.hpp>
#include <ac/ModelInfo.hpp>

#include <ac/Model.hpp>
#include <ac/Instance.hpp>

#include <astl/move.hpp>

#include <jalog/Instance.hpp>
#include <jalog/sinks/ColorSink.hpp>

#include <string>
#include <optional>
#include <latch>
#include <thread>

struct GlobalFixture {
    jalog::Instance jl;
    GlobalFixture() {
        jl.setup().add<jalog::sinks::ColorSink>();
    }
};

GlobalFixture globalFixture;

enum TestHelperFlags {
    Add_Inference = 1,
    Add_AssetSource = 2,
    No_ProviderThreads = 4,
};

struct AcTestHelper {
    ac::LocalProvider provider;
    ac::CallbackResult<ac::ModelPtr> modelResult;
    std::optional<std::latch> latch;
    float progress;
    DummyLocalInferenceModelLoader modelLoader;
    ac::CallbackResult<ac::InstancePtr> instanceResult;

    AcTestHelper(int flags)
        : provider((flags & No_ProviderThreads) ? ac::LocalProvider::No_LaunchThreads : ac::LocalProvider::Default_Init)
    {
        if (flags & Add_Inference) {
            provider.addLocalInferenceLoader("dummy", modelLoader);
        }
        if (flags & Add_AssetSource) {
            provider.addAssetSource(createDummyAssetSource(), 0);
        }

        provider.addModel(ac::ModelInfo{"empty"});
        provider.addModel(ac::ModelInfo{
            .id = "single-asset",
            .inferenceType = "dummy",
            .assets = {
                {"bad", "bad-tag"},
            }
        });
        provider.addModel(ac::ModelInfo{
            .id = "bad-assets",
            .inferenceType = "dummy",
            .assets = {
                {"asset1", "tag1"},
                {"bad-asset", "bad-tag"},
            }
        });
        provider.addModel(ac::ModelInfo{
            .id = "bad-remote-assets",
            .inferenceType = "dummy",
            .assets = {
                {"asset1", "tag1"},
                {"bad-remote-asset", "bad-tag"},
            }
        });
        provider.addModel(ac::ModelInfo{
            .id = "model",
            .inferenceType = "dummy",
            .assets = {
                {"asset1", "tag1"},
                {"asset2", "tag2"}
            }
        });
    }

    void createModelAndWait(std::string_view type, ac::Dict params) {
        latch.emplace(1);

        provider.createModel(type, astl::move(params), {
            [&](ac::CallbackResult<ac::ModelPtr> result) {
                modelResult = astl::move(result);
                latch->count_down();
            },
            [&](std::string_view, float f) {
                progress = f;
            }
        });

        latch->wait();
    }

    void createInstanceAndWait(ac::Dict params) {
        latch.emplace(1);
        auto model = modelResult.value_or(ac::ModelPtr{});
        REQUIRE(model);
        model->createInstance("general", astl::move(params), {
            [&](ac::CallbackResult<ac::InstancePtr> result) {
                instanceResult = astl::move(result);
                latch->count_down();
            }
        });
        latch->wait();
    }
};

TEST_CASE("invalid args") {
    AcTestHelper h(0);
    h.createModelAndWait("foo", {});

    REQUIRE(h.modelResult.has_error() == true);
    CHECK(h.modelResult.error().text == "Unknown model id");
}

TEST_CASE("no models") {
    AcTestHelper h(0);
    h.createModelAndWait("empty", {});

    REQUIRE(h.modelResult.has_error() == true);
    CHECK(h.modelResult.error().text == "Unknown model type");
}

TEST_CASE("missing model provider") {
    AcTestHelper h(0);
    h.createModelAndWait("model", {});

    REQUIRE(h.modelResult.has_error() == true);
    CHECK(h.modelResult.error().text == "Unknown model type");
}

TEST_CASE("not enough assets") {
    AcTestHelper h(Add_Inference);
    h.createModelAndWait("single-asset", {});

    REQUIRE(h.modelResult.has_error() == true);
    CHECK(h.modelResult.error().text == "expected 2 assets, got 1");
}

TEST_CASE("bad assets") {
    AcTestHelper h(Add_Inference|Add_AssetSource);
    h.createModelAndWait("bad-assets", {});

    REQUIRE(h.modelResult.has_error() == true);
    CHECK(h.modelResult.error().text == "asset error: Asset not found");
}

TEST_CASE("bad remote assets") {
    AcTestHelper h(Add_Inference | Add_AssetSource);
    h.createModelAndWait("bad-remote-assets", {});

    REQUIRE(h.modelResult.has_error() == true);
    CHECK(h.modelResult.error().text == "asset error: Bad remote asset");
}


TEST_CASE("model loading error") {
    AcTestHelper h(Add_Inference | Add_AssetSource);
    h.createModelAndWait("model", {{"error", true}});

    REQUIRE(h.modelResult.has_error() == true);
    CHECK(h.modelResult.error().text == "Model couldn't be loaded!");
    CHECK(h.progress == 0.5f);
}

TEST_CASE("model loading success") {
    AcTestHelper h(Add_Inference | Add_AssetSource);
    h.createModelAndWait("model", {});

    REQUIRE(h.modelResult.has_value() == true);
    CHECK(h.modelResult.value());
    CHECK(h.progress == 1.f);
}

TEST_CASE("instance loading error") {
    AcTestHelper h(Add_Inference | Add_AssetSource);
    h.createModelAndWait("model", {});
    h.createInstanceAndWait({ {"error", true} });

    REQUIRE(h.instanceResult.has_error() == true);
    CHECK(h.instanceResult.error().text == "Instance couldn't be created!");
}

TEST_CASE("instance loading success") {
    AcTestHelper h(Add_Inference | Add_AssetSource);
    h.createModelAndWait("model", {});
    h.createInstanceAndWait({});

    REQUIRE(h.instanceResult.has_value() == true);
    CHECK(h.instanceResult.value());
}

TEST_CASE("run ops") {
    AcTestHelper h(Add_Inference | Add_AssetSource);
    h.createModelAndWait("model", {});
    h.createInstanceAndWait({});

    REQUIRE(h.instanceResult.has_value() == true);
    CHECK(h.instanceResult.value());

    auto instance = h.instanceResult.value();
    std::string opError;
    ac::Dict opResult;
    std::vector<float> progress;
    unsigned resultsCount = 0;

    ac::Instance::OpCallback cb = {
        [&](ac::CallbackResult<void> result) {
            if (result.has_error()) {
                opError = result.error().text;
            }
        },
        [&](ac::Dict result) {
            opResult[resultsCount++] = result;
        },
        [&](std::string_view tag,float p) {
            CHECK(tag == "stream");
            progress.push_back(p);
        }
    };


    instance->runOp("insta", {}, cb);
    instance->synchronize();

    CHECK(resultsCount == 1);
    CHECK(opResult[0]["insta"] == "success");
    CHECK(progress == std::vector<float>{0.1f});

    resultsCount = 0;
    opResult = {};
    progress.clear();

    instance->runOp("more", {}, cb);
    instance->synchronize();

    CHECK(resultsCount == 2);
    CHECK(opResult[0]["some"] == 42);
    CHECK(opResult[1]["more"] == 1024);
    CHECK(progress == std::vector<float>{0.1f, 0.5f});
}

TEST_CASE("no threads") {
    AcTestHelper h(Add_Inference | Add_AssetSource | No_ProviderThreads);

    std::thread asset = std::thread([&]() {
        h.provider.runAssetManagement();
    });
    std::thread inference = std::thread([&]() {
        h.provider.runInference();
    });

    h.createModelAndWait("model", {});
    h.createInstanceAndWait({});

    h.provider.abortWorkers();

    REQUIRE(h.instanceResult.has_value() == true);
    CHECK(h.instanceResult.value());

    inference.join();
    asset.join();
}
