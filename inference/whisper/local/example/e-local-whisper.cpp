// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/LocalProvider.hpp>
#include <ac/LocalWhisper.hpp>
#include <ac/asset/AssetSourceLocalDir.hpp>

#include <ac/Model.hpp>
#include <ac/ModelInfo.hpp>
#include <ac/Instance.hpp>
#include <ac/Dict.hpp>

#include <jalog/Instance.hpp>
#include <jalog/sinks/ColorSink.hpp>

#include <iostream>
#include <optional>
#include <latch>

#include "ac-test-data-whisper-dir.h"

#include "ac-audio.hpp"

ac::Blob convertF32ToBlob(std::span<float> f32data) {
    ac::Blob blob;
    blob.resize(f32data.size() * sizeof(float));
    memcpy(blob.data(), f32data.data(), blob.size());
    return blob;
}

int main() {
    jalog::Instance jl;
    jl.setup().add<jalog::sinks::ColorSink>();

    ac::LocalProvider provider;
    ac::addLocalWhisperInference(provider);
    provider.addAssetSource(ac::asset::AssetSourceLocalDir_Create(AC_TEST_DATA_WHISPER_DIR), 0);

    provider.addModel(ac::ModelInfo{
        .id = "whisper_en",
        .inferenceType = "whisper.cpp",
        .assets = {{"whisper-base.en-f16.bin", {}}}
    });

    std::optional<std::latch> latch;

    ac::CallbackResult<ac::ModelPtr> modelResult;
    latch.emplace(1);
    provider.createModel("whisper_en", {}, {
        [&](ac::CallbackResult<ac::ModelPtr> result) {
            modelResult = std::move(result);
            latch->count_down();
        },
        {} // empty progress callback
    });

    latch->wait();

    if (modelResult.has_error()) {
        std::cout << "Error: Model load error: " << modelResult.error().text << "\n";
        return 1;
    }
    auto model = std::move(modelResult.value());

    ac::CallbackResult<ac::InstancePtr> instanceResult;
    latch.emplace(1);
    model->createInstance("general", {}, {
        [&](ac::CallbackResult<ac::InstancePtr> result) {
            instanceResult = std::move(result);
            latch->count_down();
        },
        {} // empty progress callback
    });

    latch->wait();
    if (instanceResult.has_error()) {
        std::cout << "Error: Instance create error: " << instanceResult.error().text << "\n";
        return 1;
    }
    auto instance = std::move(instanceResult.value());

    std::string modelBinFilePath = AC_TEST_DATA_WHISPER_DIR "/as-she-sat.wav";
    auto pcmf32 = ac::audio::loadWavF32Mono(modelBinFilePath);
    auto audioBlob = convertF32ToBlob(pcmf32);

    std::cout << "Local-whisper: Transcribing the audio [" << modelBinFilePath << "]: \n\n";

    std::string opError;
    latch.emplace(1);

    instance->runOp("transcribe", {{"audioBinaryMono", ac::Dict::binary(std::move(audioBlob))}}, {
        [&](ac::CallbackResult<void> result) {
            if (result.has_error()) {
                opError = std::move(result.error().text);
                return;
            }
            latch->count_down();
        },
        [](std::string_view, ac::Dict result) {
            std::cout << result.at("result").get<std::string_view>();
        }
    });

    latch->wait();

    std::cout << "\n";

    if (!opError.empty()) {
        std::cout << "run error: " << opError << "\n";
        return 1;
    }

    return 0;
}
