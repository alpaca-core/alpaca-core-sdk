// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/LocalProvider.hpp>
#include <ac/LocalDummy.hpp>
#include <ac/ModelInfo.hpp>
#include <ac/asset/SourceLocalDir.hpp>

#include <ac/Model.hpp>
#include <ac/Instance.hpp>

#include <jalog/Instance.hpp>
#include <jalog/sinks/ColorSink.hpp>

#include <iostream>
#include <optional>
#include <latch>

#include "ac-test-data-dummy-models.h"

int main() {
    jalog::Instance jl;
    jl.setup().add<jalog::sinks::ColorSink>();

    ac::LocalProvider provider;
    ac::addLocalDummyInference(provider);
    provider.addAssetSource(ac::asset::AssetSourceLocalDir_Create(AC_TEST_DATA_DUMMY_DIR), 0);
    provider.addModel(ac::ModelInfo{
        .id = "dummy-large",
        .inferenceType = "dummy",
        .assets = {
            {AC_DUMMY_MODEL_LARGE_ASSET, "x"},
        }
    });

    std::optional<std::latch> latch;

    ac::ModelPtr model;
    latch.emplace(1);
    provider.createModel("dummy-large", {}, {
        [&](ac::CallbackResult<ac::ModelPtr> result) {
            if (result.has_error()) {
                std::cerr << "model load error: " << result.error().text << "\n";
            }
            else {
                model = std::move(result.value());
            }
            latch->count_down();
        },
        {}
    });
    latch->wait();

    if (!model) return 1;

    ac::InstancePtr instance;
    latch.emplace(1);
    model->createInstance("general", {}, {
        [&](ac::CallbackResult<ac::InstancePtr> result) {
            if (result.has_error()) {
                std::cerr << "instance load error: " << result.error().text << "\n";
            }
            else {
                instance = std::move(result.value());
            }
            latch->count_down();
        },
        {} // empty progress callback
        });
    latch->wait();

    if (!instance) return 2;

    std::string inferenceResult;

    latch.emplace(1);
    instance->runOp("run", {{"input", {"JFK", "said:"}}, {"splice", false}}, {
        [&](ac::CallbackResult<void> result) {
            if (result.has_error()) {
                std::cerr << "run error: " << result.error().text << "\n";
            }
            latch->count_down();
        },
        [&](std::string_view, ac::Dict result) {
            inferenceResult += result["result"].get<std::string>();
        }
    });
    latch->wait();

    std::cout << "\n";

    if (inferenceResult.empty()) return 3;

    std::cout << "Inference result:\n" << inferenceResult << "\n";

    return 0;
}
