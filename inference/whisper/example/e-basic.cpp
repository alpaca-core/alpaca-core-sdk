// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

// trivial example of using alpaca-core's whisper inference

// whisper
#include <ac/whisper/Init.hpp>
#include <ac/whisper/Model.hpp>
#include <ac/whisper/Instance.hpp>

#include <ac-audio.hpp>

// logging
#include <jalog/Instance.hpp>
#include <jalog/sinks/ColorSink.hpp>

// model source directory
#include "ac-test-data-whisper-dir.h"

#include <iostream>
#include <string>
#include <vector>

int main() try {
    jalog::Instance jl;
    jl.setup().add<jalog::sinks::ColorSink>();

    std::cout << "Basic example\n";

    // initialize the library
    ac::whisper::initLibrary();

    // load model
    std::string modelBinFilePath = AC_TEST_DATA_WHISPER_DIR "/whisper-base.en-f16.bin";
    ac::whisper::Model model(modelBinFilePath.c_str(), {});

    // // create inference instance
    ac::whisper::Instance instance(model, {});

    std::string audioFile = AC_TEST_DATA_WHISPER_DIR "/as-she-sat.wav";
    std::vector<float> pcmf32;               // mono-channel F32 PCM

    std::cout << "Transcribing the audio [" << audioFile << "]: \n\n";

    // transcript the audio
    instance.runOp("transcribe", pcmf32, [](std::string res){
        std::cout<< res <<'\n';
    });

    return 0;
}
catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
