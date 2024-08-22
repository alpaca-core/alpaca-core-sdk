// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ac-audio.hpp"

// third-party utilities
// use your favorite implementations
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#define COMMON_SAMPLE_RATE 16000

namespace {
bool is_wav_buffer(const std::string buf) {
    // RIFF ref: https://en.wikipedia.org/wiki/Resource_Interchange_File_Format
    // WAV ref: https://www.mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
    if (buf.size() < 12 || buf.substr(0, 4) != "RIFF" || buf.substr(8, 4) != "WAVE") {
        return false;
    }

    uint32_t chunk_size = *reinterpret_cast<const uint32_t*>(buf.data() + 4);
    if (chunk_size + 8 != buf.size()) {
        return false;
    }

    return true;
}
}

namespace ac::audio {
bool readWav(
    const std::string& fileName,
    std::vector<float>& pcmf32,
    std::vector<std::vector<float>>& pcmf32s,
    bool stereo) {

    drwav wav;
    std::vector<uint8_t> wavData; // used for pipe input from stdin or ffmpeg decoding output

    if (fileName == "-") {
        {
            #ifdef _WIN32
            _setmode(_fileno(stdin), _O_BINARY);
            #endif

            uint8_t buf[1024];
            while (true)
            {
                const size_t n = fread(buf, 1, sizeof(buf), stdin);
                if (n == 0) {
                    break;
                }
                wavData.insert(wavData.end(), buf, buf + n);
            }
        }

        if (drwav_init_memory(&wav, wavData.data(), wavData.size(), nullptr) == false) {
            fprintf(stderr, "error: failed to open WAV file from stdin\n");
            return false;
        }

        fprintf(stderr, "%s: read %zu bytes from stdin\n", __func__, wavData.size());
    }
    else if (is_wav_buffer(fileName)) {
        if (drwav_init_memory(&wav, fileName.c_str(), fileName.size(), nullptr) == false) {
            fprintf(stderr, "error: failed to open WAV file from fileName buffer\n");
            return false;
        }
    }
    else if (drwav_init_file(&wav, fileName.c_str(), nullptr) == false) {
#if defined(WHISPER_FFMPEG)
        if (ffmpeg_decode_audio(fileName, wavData) != 0) {
            fprintf(stderr, "error: failed to ffmpeg decode '%s' \n", fileName.c_str());
            return false;
        }
        if (drwav_init_memory(&wav, wavData.data(), wavData.size(), nullptr) == false) {
            fprintf(stderr, "error: failed to read wav data as wav \n");
            return false;
        }
#else
        fprintf(stderr, "error: failed to open '%s' as WAV file\n", fileName.c_str());
        return false;
#endif
    }

    if (wav.channels != 1 && wav.channels != 2) {
        fprintf(stderr, "%s: WAV file '%s' must be mono or stereo\n", __func__, fileName.c_str());
        drwav_uninit(&wav);
        return false;
    }

    if (stereo && wav.channels != 2) {
        fprintf(stderr, "%s: WAV file '%s' must be stereo for diarization\n", __func__, fileName.c_str());
        drwav_uninit(&wav);
        return false;
    }

    if (wav.sampleRate != COMMON_SAMPLE_RATE) {
        fprintf(stderr, "%s: WAV file '%s' must be %i kHz\n", __func__, fileName.c_str(), COMMON_SAMPLE_RATE/1000);
        drwav_uninit(&wav);
        return false;
    }

    if (wav.bitsPerSample != 16) {
        fprintf(stderr, "%s: WAV file '%s' must be 16-bit\n", __func__, fileName.c_str());
        drwav_uninit(&wav);
        return false;
    }

    const uint64_t n = wavData.empty() ? wav.totalPCMFrameCount : wavData.size()/(wav.channels*wav.bitsPerSample/8);

    std::vector<int16_t> pcm16;
    pcm16.resize(n*wav.channels);
    drwav_read_pcm_frames_s16(&wav, n, pcm16.data());
    drwav_uninit(&wav);

    // convert to mono, float
    pcmf32.resize(n);
    if (wav.channels == 1) {
        for (uint64_t i = 0; i < n; i++) {
            pcmf32[i] = float(pcm16[i])/32768.0f;
        }
    } else {
        for (uint64_t i = 0; i < n; i++) {
            pcmf32[i] = float(pcm16[2*i] + pcm16[2*i + 1])/65536.0f;
        }
    }

    if (stereo) {
        // convert to stereo, float
        pcmf32s.resize(2);

        pcmf32s[0].resize(n);
        pcmf32s[1].resize(n);
        for (uint64_t i = 0; i < n; i++) {
            pcmf32s[0][i] = float(pcm16[2*i])/32768.0f;
            pcmf32s[1][i] = float(pcm16[2*i + 1])/32768.0f;
        }
    }

    return true;
}

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
}
