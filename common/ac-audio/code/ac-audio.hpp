// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"

#include <string>
#include <vector>
#include <fstream>

#include <itlib/span.hpp>

namespace ac::audio {
AC_AUDIO_EXPORT std::vector<int16_t> loadWavI16Mono(const std::string& path);
AC_AUDIO_EXPORT std::vector<float> loadWavF32Mono(const std::string& path);
AC_AUDIO_EXPORT std::vector<float> convertWavI16ToF32(itlib::span<const int16_t> i16);

// Write PCM data into WAV audio file
class AC_AUDIO_EXPORT WavWriter {
public:
    bool open(const std::string& filename,
              const uint32_t sampleRate,
              const uint16_t bitsPerSample,
              const uint16_t channels);

    bool close();

    bool write(const int16_t* data, size_t length);

    ~WavWriter();
private:
    bool openWav(const std::string& filename);

    bool writeHeader(const uint32_t sampleRate,
                      const uint16_t bitsPerSample,
                      const uint16_t channels);

    // It is assumed that PCM data is normalized to a range from -1 to 1
    bool writeAudio(const int16_t* data, size_t length);

    std::ofstream m_file;
    uint32_t m_dataSize = 0;
    std::string m_wavFilename;

};

}
