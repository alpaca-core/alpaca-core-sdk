// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ac-audio.hpp"

// third-party utilities
// use your favorite implementations
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#include <itlib/sentry.hpp>
#include <itlib/span.hpp>
#include <astl/throw_ex.hpp>

namespace ac::audio {
bool WavWriter::open(
    const std::string& filename,
    const uint32_t sampleRate,
    const uint16_t bitsPerSample,
    const uint16_t channels) {
    if (openWav(filename)) {
        writeHeader(sampleRate, bitsPerSample, channels);
    } else {
        return false;
    }

    return true;
}

bool WavWriter::close() {
    if (m_file.is_open()) {
        m_file.close();
    }
    return true;
}

bool WavWriter::write(const float* data, size_t length) {
    return writeAudio(data, length);
}

WavWriter::~WavWriter() {
    close();
}

bool WavWriter::openWav(const std::string& filename) {
    if (filename != m_wavFilename) {
        if (m_file.is_open()) {
            m_file.close();
        }
    }
    if (!m_file.is_open()) {
        m_file.open(filename, std::ios::binary);
        m_wavFilename = filename;
        m_dataSize = 0;
    }
    return m_file.is_open();
}

bool WavWriter::writeHeader(
    const uint32_t sampleRate,
    const uint16_t bitsPerSample,
    const uint16_t channels) {

    m_file.write("RIFF", 4);
    m_file.write("\0\0\0\0", 4);    // Placeholder for file size
    m_file.write("WAVE", 4);
    m_file.write("fmt ", 4);

    const uint32_t sub_chunk_size = 16;
    const uint16_t audio_format = 1;      // PCM format
    const uint32_t byte_rate = sampleRate * channels * bitsPerSample / 8;
    const uint16_t block_align = channels * bitsPerSample / 8;

    m_file.write(reinterpret_cast<const char *>(&sub_chunk_size), 4);
    m_file.write(reinterpret_cast<const char *>(&audio_format), 2);
    m_file.write(reinterpret_cast<const char *>(&channels), 2);
    m_file.write(reinterpret_cast<const char *>(&sampleRate), 4);
    m_file.write(reinterpret_cast<const char *>(&byte_rate), 4);
    m_file.write(reinterpret_cast<const char *>(&block_align), 2);
    m_file.write(reinterpret_cast<const char *>(&bitsPerSample), 2);
    m_file.write("data", 4);
    m_file.write("\0\0\0\0", 4);    // Placeholder for data size

    return true;
}

bool WavWriter::writeAudio(const float* data, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        const int16_t intSample = int16_t(data[i] * 32767);
        m_file.write(reinterpret_cast<const char *>(&intSample), sizeof(int16_t));
        m_dataSize += sizeof(int16_t);
    }
    if (m_file.is_open()) {
        m_file.seekp(4, std::ios::beg);
        uint32_t fileSize = 36 + m_dataSize;
        m_file.write(reinterpret_cast<char *>(&fileSize), 4);
        m_file.seekp(40, std::ios::beg);
        m_file.write(reinterpret_cast<char *>(&m_dataSize), 4);
        m_file.seekp(0, std::ios::end);
    }
    return true;
}

std::pair<std::vector<int16_t>, int> loadWavI16(const std::string& path) {
    drwav wav;
    if (!drwav_init_file(&wav, path.c_str(), nullptr)) {
        throw_ex{} << "Failed to load wav file: " << path;
    }

    itlib::sentry uninitDrWav{[&] { drwav_uninit(&wav); }};

    if (wav.channels != 1 && wav.channels != 2) {
        throw_ex{} << "Unsupported number of channels: " << wav.channels;
    }

    const uint32_t commonSampleRate = 16000;
    if (wav.sampleRate != commonSampleRate) {
        throw_ex{} << "Unsupported sample rate: " << wav.sampleRate;
    }

    if (wav.bitsPerSample != 16) {
        throw_ex{} << "Unsupported bits per sample: " << wav.bitsPerSample;
    }

    std::vector<int16_t> pcms16(wav.totalPCMFrameCount * wav.channels);
    drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, pcms16.data());

    return { std::move(pcms16), wav.channels };
}

std::vector<int16_t> loadWavI16Mono(const std::string& path) {
    const auto [pcms16, channels] = loadWavI16(path);
    if (channels == 1) {
        return pcms16;
    }

    auto n = pcms16.size() / channels;
    std::vector<int16_t> pcmmono(n);
    auto ppcms16 = pcms16.data();
    auto ppcmmono = pcmmono.data();
    while (n-- > 0) {
        int32_t combined = *ppcms16++;
        combined += *ppcms16++;
        combined /= 2;
        *ppcmmono++ = int16_t(combined);
    }

    // sanity
    assert(ppcmmono == pcmmono.data() + pcmmono.size() && ppcms16 == pcms16.data() + pcms16.size());

    return pcmmono;
}

std::vector<float> loadWavF32Mono(const std::string& path) {
    const auto [pcms16, channels] = loadWavI16(path);

    auto n = pcms16.size() / channels;
    std::vector<float> pcmf32(n);
    auto ppcms16 = pcms16.data();
    auto ppcmf32 = pcmf32.data();

    // TODO: check if we're correct to add 1 and 2 to scale making it never reach 1.000
    if (channels == 1) {
        constexpr float SCALE = 1 /* here */ + float(std::numeric_limits<int16_t>::max());
        while (n-- > 0) {
            *ppcmf32++ = *ppcms16++ / SCALE;
        }
    }
    else {
        constexpr float SCALE = 2 /* and here */ + 2 * float(std::numeric_limits<int16_t>::max());
        while (n-- > 0) {
            *ppcmf32 = *ppcms16++ / SCALE;
            *ppcmf32++ = *ppcms16++ / SCALE;
        }
    }

    // sanity
    assert(ppcmf32 == pcmf32.data() + pcmf32.size() && ppcms16 == pcms16.data() + pcms16.size());

    return pcmf32;
}

std::vector<float> convertWavI16ToF32(itlib::span<const int16_t> i16) {
    std::vector<float> f32(i16.size());
    auto pi16 = i16.data();
    auto pf32 = f32.data();
    while (pi16 != i16.end()) {
        constexpr float SCALE = 1 /* here */ + float(std::numeric_limits<int16_t>::max());
        *pf32++ = *pi16++ / SCALE;
    }
    return f32;
}

}
