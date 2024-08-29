// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/whisper/Init.hpp>
#include <ac/whisper/Model.hpp>
#include <ac/whisper/Instance.hpp>

#include <ac-audio.hpp>

#include <doctest/doctest.h>

#include "ac-test-data-whisper-dir.h"


struct GlobalFixture {
    GlobalFixture() {
        ac::whisper::initLibrary();
    }
};

GlobalFixture globalFixture;

const char* Base_en_f16 = AC_TEST_DATA_WHISPER_DIR "/whisper-base.en-f16.bin";

TEST_CASE("inference") {
    ac::whisper::Model model(Base_en_f16, {});

    auto& params = model.params();
    CHECK(params.gpu);

    // general inference
    {
        ac::whisper::Instance inst(model, {});
        REQUIRE(!!inst.context());

        inst.runOp("inference", std::span<float>(), [](std::string result) {
            CHECK(result == "OK");
        });

        std::string prenticeHallText(
            "Yes, I like it. Prentice Hall always delivers good seminars."
            "All of its speakers are very well known and also very knowledgeable in the subject matter. Did you attend the seminar on leadership in Long Beach last January?");

        std::string audioFilePath = AC_TEST_DATA_WHISPER_DIR "/prentice-hall.wav";
        auto pcmf32 = ac::audio::loadWavF32Mono(audioFilePath);

        CHECK(pcmf32.size() > 0);

        inst.runOp("inference", pcmf32, [&prenticeHallText](std::string result) {
            CHECK(result == prenticeHallText);
        });

    }
}
