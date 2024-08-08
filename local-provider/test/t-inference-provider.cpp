// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

#include <doctest/doctest.h>
#include <ac/LocalInference.hpp>
#include <ac/LocalProvider.hpp>

#include <ac/Model.hpp>
#include <ac/Instance.hpp>

#include <iostream>
#include <string>
#include <optional>
#include <latch>

class DummyLocalInferenceInstance final : public ac::LocalInferenceInstance {
public:
    virtual void runOpSync(std::string_view op, ac::Dict params, std::function<void(ac::Dict)> streamCb) override {
        if (op == "insta") {
            streamCb({ {"insta", "success"} });
            return;
        }

        streamCb({ {"some", 42} });

        if (op == "error") {
            streamCb({ {"error", params.at("error").get<std::string>() } });
            return;
        }
        else if (op == "more") {
            streamCb({ {"more", 1024} });
        }
    }
};

class DummyLocalInferenceModel final : public ac::LocalInferenceModel {
public:

    virtual std::unique_ptr<ac::LocalInferenceInstance> createInstanceSync(std::string_view, ac::Dict params) override {

        if (ac::Dict_optValueAt(params, "error", false)) {
            return nullptr;
        }

        return std::unique_ptr<DummyLocalInferenceInstance >(new DummyLocalInferenceInstance());
    }
};

class DummyLocalInferenceModelLoader final : public ac::LocalInferenceModelLoader {
public:
    virtual std::unique_ptr<ac::LocalInferenceModel> loadModelSync(ac::Dict params, std::function<void(float)> progress) override {
        progress(0.2f);
        progress(0.5f);

        if (ac::Dict_optValueAt(params, "error", false)) {
            return nullptr;
        }

        progress(1.f);
        return std::unique_ptr<DummyLocalInferenceModel>(new DummyLocalInferenceModel());
    }
};

#define AC_TEST_BEGINNING                               \
    ac::LocalProvider provider;                         \
    ac::CallbackResult<ac::ModelPtr> modelResult;       \
    std::optional<std::latch> latch;                    \
    float progress;                                     \
    DummyLocalInferenceModelLoader modelLoader;         \
    ac::CallbackResult<ac::InstancePtr> instanceResult; \

void createModelAndWait(
    ac::LocalProvider& provider,
    const ac::Dict& args,
    ac::CallbackResult<ac::ModelPtr>& modelResult,
    float& progress,
    std::optional<std::latch>& latch) {
    latch.emplace(1);

    provider.createModel(args, {
        [&](ac::CallbackResult<ac::ModelPtr> result) {
            modelResult = std::move(result);
            latch->count_down();
        },
        [&](float f) {
            progress = f;
            std::cout << "model load progress: " << f << "\n";
        }
    });

    latch->wait();
}

TEST_CASE("invalid args") {
    AC_TEST_BEGINNING

    createModelAndWait(
        provider,
        {{}},
        modelResult,
        progress,
        latch);

    CHECK(modelResult.has_value() == false);
    CHECK(modelResult.has_error() == true);
    CHECK(modelResult.error().text == "[json.exception.type_error.304] cannot use at() with array");
}

TEST_CASE("missing model provider") {
    AC_TEST_BEGINNING

    createModelAndWait(
        provider,
        {{ "type", "llama.cpp" }, {"error", true}},
        modelResult,
        progress,
        latch);

    CHECK(modelResult.has_value() == false);
    CHECK(modelResult.has_error() == true);
    CHECK(modelResult.error().text == "Unknown model type");
}

TEST_CASE("model loading error") {
    AC_TEST_BEGINNING

    provider.addLocalInferenceLoader("llama.cpp", modelLoader);
    createModelAndWait(
        provider,
        {{ "type", "llama.cpp" }, {"error", true}},
        modelResult,
        progress,
        latch);

    CHECK(modelResult.has_value() == false);
    CHECK(modelResult.has_error() == true);
    CHECK(modelResult.error().text == "Model couldn't be loaded!");
    CHECK(progress == 0.5f);
}

TEST_CASE("model loading success") {
    AC_TEST_BEGINNING

    provider.addLocalInferenceLoader("llama.cpp", modelLoader);
    createModelAndWait(
        provider,
        {{ "type", "llama.cpp" }},
        modelResult,
        progress,
        latch);

    CHECK(modelResult.has_value() == true);
    CHECK(modelResult.value() != nullptr);
    CHECK(modelResult.has_error() == false);
    CHECK(progress == 1.f);
}

void createInstanceAndWait(
    ac::ModelPtr model,
    ac::Dict params,
    ac::CallbackResult<ac::InstancePtr>& instanceResult,
    std::optional<std::latch>& latch) {
    latch.emplace(1);
    model->createInstance("general", params, {
        [&](ac::CallbackResult<ac::InstancePtr> result){
            instanceResult = std::move(result);
            latch->count_down();
        },
        {} // no progress callback
    });
    latch->wait();
}

TEST_CASE("instance loading error") {
    AC_TEST_BEGINNING

    provider.addLocalInferenceLoader("llama.cpp", modelLoader);
    createModelAndWait(
        provider,
        {{ "type", "llama.cpp" }},
        modelResult,
        progress,
        latch);

    createInstanceAndWait(
        modelResult.value(),
        {{"error", true}},
        instanceResult,
        latch);

    CHECK(instanceResult.has_value() == false);
    CHECK(instanceResult.has_error() == true);
    CHECK(instanceResult.error().text == "Instance couldn't be created!");
}

TEST_CASE("instance loading success") {
    AC_TEST_BEGINNING

    provider.addLocalInferenceLoader("llama.cpp", modelLoader);
    createModelAndWait(
        provider,
        {{ "type", "llama.cpp" }},
        modelResult,
        progress,
        latch);

    createInstanceAndWait(
        modelResult.value(),
        {{}},
        instanceResult,
        latch);

    CHECK(instanceResult.has_value() == true);
    CHECK(instanceResult.value() != nullptr);
    CHECK(instanceResult.has_error() == false);
}

TEST_CASE("run ops") {
    AC_TEST_BEGINNING

    provider.addLocalInferenceLoader("llama.cpp", modelLoader);
    createModelAndWait(
        provider,
        {{ "type", "llama.cpp" }},
        modelResult,
        progress,
        latch);

    createInstanceAndWait(
        modelResult.value(),
        {{}},
        instanceResult,
        latch);

    auto instance = instanceResult.value();
    std::string opError;
    ac::Dict opResult;
    unsigned resultsCount = 0;

    latch.emplace(1);
    instance->runOp("insta", {{}}, {
        [&](ac::CallbackResult<void> result) {
            if (result.has_error()) {
                opError = std::move(result.error().text);
                return;
            }
            latch->count_down();
        },
        [&](ac::Dict result) {
            opResult[resultsCount++] = result;
        }
    });
    latch->wait();

    CHECK(resultsCount == 1);
    CHECK(opResult[0]["insta"] == "success");

    resultsCount = 0;
    opResult = {};

    latch.emplace(1);
    instance->runOp("more", {{}}, {
        [&](ac::CallbackResult<void> result) {
            if (result.has_error()) {
                opError = std::move(result.error().text);
                return;
            }
            latch->count_down();
        },
        [&](ac::Dict result) {
            opResult[resultsCount++] = result;
        }
    });
    latch->wait();

    CHECK(resultsCount == 2);
    CHECK(opResult[0]["some"] == 42);
    CHECK(opResult[1]["more"] == 1024);
}


