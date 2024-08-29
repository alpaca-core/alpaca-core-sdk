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
whisper_context_params whisperFromModelParams(const Model::Params& params)
{
    whisper_context_params whisperParams = whisper_context_default_params();
    whisperParams.use_gpu = params.gpu;

    return whisperParams;
}

whisper_sampling_strategy whisperFromACStrategy(Instance::InitParams::SamplingStrategy strategy) {
    switch (strategy)
    {
    case Instance::InitParams::SamplingStrategy::GREEDY:
        return whisper_sampling_strategy::WHISPER_SAMPLING_GREEDY;
    case Instance::InitParams::SamplingStrategy::BEAM_SEARCH:
        return whisper_sampling_strategy::WHISPER_SAMPLING_BEAM_SEARCH;
    default:
        throw_ex{} << "Unknown sampling strategy!";;
    }
}

whisper_full_params whisperFromInstanceParams(Instance::InitParams& iparams) {
    // Those parameters are based on the whisper.cpp main example,
    // some of them are removed since they are irrelevant for the current use case
    // https://github.com/alpaca-core/whisper.cpp/blob/6739eb83c3ca5cf40d24c6fe8442a761a1eb6248/examples/main/main.cpp#L30

    struct whisper_params {
        int32_t n_threads     = 16;//std::min(4, (int32_t) std::thread::hardware_concurrency());
        int32_t n_processors  = 1;
        int32_t offset_t_ms   = 0;
        int32_t offset_n      = 0;
        int32_t duration_ms   = 0;
        int32_t progress_step = 5;
        int32_t max_context   = -1;
        int32_t max_len       = 0;
        int32_t best_of       = whisper_full_default_params(WHISPER_SAMPLING_GREEDY).greedy.best_of;
        int32_t beam_size     = whisper_full_default_params(WHISPER_SAMPLING_BEAM_SEARCH).beam_search.beam_size;
        int32_t audio_ctx     = 0;

        float word_thold      =  0.01f;
        float entropy_thold   =  2.40f;
        float logprob_thold   = -1.00f;
        float grammar_penalty = 100.0f;
        float temperature     = 0.0f;
        float temperature_inc = 0.2f;

        bool detect_language = false;
        std::string language  = "en";
        bool split_on_word   = false;
        bool diarize         = false;
        bool tinydiarize     = false;
        bool translate       = false;
        bool print_special   = false;
        bool print_progress  = false;
        bool no_timestamps   = false;
        std::string prompt;

    } params;

    // The params setup is based the main example of whisper.cpp
    // https://github.com/alpaca-core/whisper.cpp/blob/6739eb83c3ca5cf40d24c6fe8442a761a1eb6248/examples/main/main.cpp#L1084

    whisper_full_params wparams = whisper_full_default_params(whisperFromACStrategy(iparams.samplingStrategy));

    wparams.print_realtime   = false;
    wparams.print_progress   = params.print_progress;
    wparams.print_timestamps = !params.no_timestamps;
    wparams.print_special    = params.print_special;
    wparams.translate        = params.translate;
    wparams.language         = params.language.c_str();
    wparams.detect_language  = params.detect_language;
    wparams.n_threads        = params.n_threads;
    wparams.n_max_text_ctx   = params.max_context >= 0 ? params.max_context : wparams.n_max_text_ctx;
    wparams.offset_ms        = params.offset_t_ms;
    wparams.duration_ms      = params.duration_ms;

    wparams.token_timestamps = params.max_len > 0;
    wparams.thold_pt         = params.word_thold;
    wparams.max_len          = params.max_len == 0 ? 60 : params.max_len;
    wparams.split_on_word    = params.split_on_word;
    wparams.audio_ctx        = params.audio_ctx;

    wparams.tdrz_enable      = params.tinydiarize; // [TDRZ]

    wparams.initial_prompt   = params.prompt.c_str();

    wparams.greedy.best_of        = params.best_of;
    wparams.beam_search.beam_size = params.beam_size;

    wparams.temperature_inc  = params.temperature_inc;
    wparams.temperature      = params.temperature;

    wparams.entropy_thold    = params.entropy_thold;
    wparams.logprob_thold    = params.logprob_thold;

    wparams.no_timestamps    = params.no_timestamps;

    return wparams;
}
}

Instance::Instance(Model& model, InitParams params)
    : m_model(model)
    , m_params(astl::move(params))
    , m_ctx(whisper_init_from_file_with_params(m_model.pathToBin().c_str(), whisperFromModelParams(m_model.params())), whisper_free)
{
        if (!m_ctx) {
        throw std::runtime_error("Failed to load model");
    }
}

Instance::~Instance() = default;

std::string Instance::transcribe(std::span<float> pcmf32) {
    if (pcmf32.empty())
    {
        // TODO: Investigate why whisper.cpp crashes if the input is empty
        return std::string();
    }

    return runInference(pcmf32);
}

std::string Instance::runInference(std::span<float> pcmf32) {
    auto wparams = whisperFromInstanceParams(m_params);
    if (whisper_full_parallel(m_ctx.get(), wparams, pcmf32.data(), pcmf32.size(), 1 /* n_processors */) != 0) {
        fprintf(stderr, "failed to process audio!\n");
        return "";
    }

    std::string result;
    const int n_segments = whisper_full_n_segments(m_ctx.get());
    for (int i = 0; i < n_segments; ++i) {
        const char * text = whisper_full_get_segment_text(m_ctx.get(), i);
        std::string speaker = "";

        result += speaker + text + "\n";
    }

    return result;
}

} // namespace ac::whisper
