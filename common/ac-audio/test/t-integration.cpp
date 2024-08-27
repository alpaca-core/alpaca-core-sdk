// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

#include <ac-audio.hpp>

#include <doctest/doctest.h>

#include "ac-test-data-whisper-dir.h"

const char* AudioFile = AC_TEST_DATA_WHISPER_DIR "/prentice-hall.wav";
const char* AudioFileSmall = AC_TEST_DATA_WHISPER_DIR "/yes.wav";

TEST_CASE("load audio f32") {
    auto data = ac::audio::loadWavF32Mono(AudioFileSmall);
    CHECK(data.size() == 9984);

    data = ac::audio::loadWavF32Mono(AudioFile);
    CHECK(data.size() == 217430);
}

TEST_CASE("load audio i16") {
    auto data = ac::audio::loadWavI16Mono(AudioFileSmall);
    CHECK(data.size() == 9984);

    data = ac::audio::loadWavI16Mono(AudioFile);
    CHECK(data.size() == 217430);
}

TEST_CASE("convert i16 to f32") {
    auto i16 = ac::audio::loadWavI16Mono(AudioFileSmall);
    auto f32 = ac::audio::convertWavI16ToF32(i16);
    CHECK(f32.size() == i16.size());

    auto loadedF32 = ac::audio::loadWavF32Mono(AudioFileSmall);
    REQUIRE(loadedF32.size() == f32.size());

    for (size_t i = 0; i < f32.size(); i++)
    {
        CHECK(f32[i] == doctest::Approx(loadedF32[i]).epsilon(0.1));
    }
}

TEST_CASE("save wav") {
    ac::audio::WavWriter writer;
    CHECK(writer.open("test.wav", 16000, 16, 1));

    std::vector<float> data(10000, 0.0f);
    CHECK(writer.write(data.data(), data.size()));
    CHECK(writer.close());
}

