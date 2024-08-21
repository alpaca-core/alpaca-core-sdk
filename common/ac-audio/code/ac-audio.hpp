// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"

#include <string>
#include <vector>
#include <fstream>

namespace ac::audio {
AC_AUDIO_EXPORT bool readWav(const std::string& fname, std::vector<float>& pcmf32, std::vector<std::vector<float>>& pcmf32s, bool stereo);

// Write PCM data into WAV audio file
class AC_AUDIO_EXPORT wavWriter {
public:
    bool open(const std::string & filename,
              const uint32_t sample_rate,
              const uint16_t bits_per_sample,
              const uint16_t channels);

    bool close();

    bool write(const float * data, size_t length);

    ~wavWriter();
private:
    bool open_wav(const std::string & filename);

    bool write_header(const uint32_t sample_rate,
                      const uint16_t bits_per_sample,
                      const uint16_t channels);

    // It is assumed that PCM data is normalized to a range from -1 to 1
    bool write_audio(const float * data, size_t length);

    std::ofstream file;
    uint32_t dataSize = 0;
    std::string wav_filename;

};

}
