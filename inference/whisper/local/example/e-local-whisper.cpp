// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/LocalWhisper.hpp>

#include <ac/local/ModelFactory.hpp>
#include <ac/local/Model.hpp>
#include <ac/local/Instance.hpp>

#include <jalog/Instance.hpp>
#include <jalog/sinks/DefaultSink.hpp>

#include <iostream>

#include "ac-test-data-whisper-dir.h"

#include <ac-audio.hpp>

ac::Blob convertF32ToBlob(std::span<float> f32data) {
    ac::Blob blob;
    blob.resize(f32data.size() * sizeof(float));
    memcpy(blob.data(), f32data.data(), blob.size());
    return blob;
}

int main() try {
    jalog::Instance jl;
    jl.setup().add<jalog::sinks::DefaultSink>();

    ac::local::ModelFactory factory;
    ac::local::addWhisperInference(factory);

    auto model = factory.createModel(
        {
            .inferenceType = "whisper.cpp",
            .assets = {
                {.path = AC_TEST_DATA_WHISPER_DIR "/whisper-base.en-f16.bin"}
            }
        },
        {}, // no params
        {} // empty progress callback
    );

    auto instance = model->createInstance("general", {});

    std::string audioFile = AC_TEST_DATA_WHISPER_DIR "/as-she-sat.wav";
    auto pcmf32 = ac::audio::loadWavF32Mono(audioFile);
    auto audioBlob = convertF32ToBlob(pcmf32);

    std::cout << "Local-whisper: Transcribing the audio [" << audioFile << "]: \n\n";

    auto result = instance->runOp("transcribe", {{"audioBinaryMono", ac::Dict::binary(std::move(audioBlob))}}, {});

    std::cout << result.at("result").get<std::string_view>() << '\n';

    return 0;
}
catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
    return 1;
}
