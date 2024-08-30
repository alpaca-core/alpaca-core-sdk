// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

#include <ac-audio.hpp>

#include <doctest/doctest.h>

#include "ac-test-data-whisper-dir.h"

const char* AudioFile = AC_TEST_DATA_WHISPER_DIR "/prentice-hall.wav";
const char* AudioFileSmall = AC_TEST_DATA_WHISPER_DIR "/yes.wav";

TEST_CASE("core") {
    // load audio f32
    auto dataf32 = ac::audio::loadWavF32Mono(AudioFileSmall);
    CHECK(dataf32.size() == 9984);

    dataf32 = ac::audio::loadWavF32Mono(AudioFile);
    CHECK(dataf32.size() == 217430);

    // load audio i16
    auto datai16 = ac::audio::loadWavI16Mono(AudioFileSmall);
    CHECK(datai16.size() == 9984);

    datai16 = ac::audio::loadWavI16Mono(AudioFile);
    CHECK(datai16.size() == 217430);

    // convert i16 to f32
    auto convertedf32 = ac::audio::convertWavI16ToF32(datai16);
    CHECK(convertedf32.size() == datai16.size());

    for (size_t i = 0; i < convertedf32.size(); i++) {
        CHECK(convertedf32[i] == doctest::Approx(dataf32[i]).epsilon(0.1));
    }

    // save wav
    ac::audio::WavWriter writer;
    CHECK(writer.open("test.wav", 16000, 16, 1));

    std::vector<float> data(10000, 0.0f);
    CHECK(writer.write(data.data(), data.size()));
    CHECK(writer.close());
}
