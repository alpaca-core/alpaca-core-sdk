// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Instance.hpp"
#include "Model.hpp"
#include "Logging.hpp"

#include <whisper.h>

#include <astl/throw_ex.hpp>
#include <astl/iile.h>
#include <astl/move.hpp>
#include <itlib/sentry.hpp>
#include <cassert>
#include <span>

namespace ac::whisper {
namespace {
whisper_sampling_strategy whisperFromACStrategy(Instance::InitParams::SamplingStrategy strategy) {
    switch (strategy) {
    case Instance::InitParams::SamplingStrategy::GREEDY:
        return whisper_sampling_strategy::WHISPER_SAMPLING_GREEDY;
    case Instance::InitParams::SamplingStrategy::BEAM_SEARCH:
        return whisper_sampling_strategy::WHISPER_SAMPLING_BEAM_SEARCH;
    default:
        throw_ex{} << "Unknown sampling strategy!";;
    }
}

whisper_full_params whisperFromInstanceParams(Instance::InitParams& iparams) {
    // The params setup is based the main example of whisper.cpp
    // https://github.com/alpaca-core/whisper.cpp/blob/6739eb83c3ca5cf40d24c6fe8442a761a1eb6248/examples/main/main.cpp#L1084
    whisper_full_params wparams = whisper_full_default_params(whisperFromACStrategy(iparams.samplingStrategy));
    wparams.print_progress   = false;
    wparams.print_timestamps = false;
    wparams.max_len          = 60;

    return wparams;
}
}

Instance::Instance(Model& model, InitParams params)
    : m_model(model)
    , m_params(astl::move(params))
    , m_state(whisper_init_state(model.context()), whisper_free_state)
{}

Instance::~Instance() = default;

std::string Instance::transcribe(std::span<float> pcmf32) {
    if (pcmf32.empty()) {
        // TODO: Investigate why whisper.cpp crashes if the input is empty #65
        return std::string();
    }

    return runInference(pcmf32);
}

std::string Instance::runInference(std::span<float> pcmf32) {
    auto wparams = whisperFromInstanceParams(m_params);

    if (whisper_full_with_state(m_model.context(), m_state.get(), wparams, pcmf32.data(), int(pcmf32.size())) != 0) {
        throw_ex{} << "Failed to process audio!";
    }

    std::string result;
    const int n_segments = whisper_full_n_segments_from_state(m_state.get());
    for (int i = 0; i < n_segments; ++i) {
        const char * text = whisper_full_get_segment_text_from_state(m_state.get(), i);
        result += std::string(text) + "\n";
    }

    return result;
}
} // namespace ac::whisper
