// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/LocalProvider.hpp>
#include <ac/LocalWhisper.hpp>

#include <ac/Model.hpp>
#include <ac/Instance.hpp>

#include <jalog/Instance.hpp>
#include <jalog/sinks/ColorSink.hpp>

#include <iostream>
#include <optional>
#include <latch>

#include "ac-test-data-whisper-dir.h"

#include "ac-audio.hpp"

int main() {
    jalog::Instance jl;
    jl.setup().add<jalog::sinks::ColorSink>();

    ac::LocalProvider provider;
    ac::addLocalWhisperInference(provider);
    provider.addLocalModel(
        "whisper_en",
        {{"type", "whisper.cpp"}, {"bin", AC_TEST_DATA_WHISPER_DIR "/whisper-base.en-f16.bin"}}
    );

    std::optional<std::latch> latch;

    ac::CallbackResult<ac::ModelPtr> modelResult;
    latch.emplace(1);
    provider.createModel("whisper_en", {}, {
        [&](ac::CallbackResult<ac::ModelPtr> result) {
            modelResult = std::move(result);
            latch->count_down();
        },
        [](float progress) {
            const int barWidth = 50;
            static float currProgress = 0;
            auto delta = int(progress * barWidth) - int(currProgress * barWidth);
            for (int i = 0; i < delta; i++) {
                std::cout.put('=');
            }
            currProgress = progress;
            if (progress == 1.f) {
                std::cout << '\n';
            }
        }
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
    std::vector<float> pcmf32;
    std::vector<std::vector<float>> pcmf32s;

    if (!ac::audio::readWav(modelBinFilePath, pcmf32, pcmf32s, false)) {
        std::cout << "Error: Couldn't read wav provided file! \n";
        return 1;
    }
    ;

    std::string opError;
    latch.emplace(1);
    instance->runOp("run", {{"audioBinaryMono", pcmf32}, {"audioBinaryStereo", pcmf32s}}, {
        [&](ac::CallbackResult<void> result) {
            if (result.has_error()) {
                opError = std::move(result.error().text);
                return;
            }
            latch->count_down();
        },
        [](ac::Dict result) {
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
