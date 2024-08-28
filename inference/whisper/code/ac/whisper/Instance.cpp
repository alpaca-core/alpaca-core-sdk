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
whisper_sampling_strategy ACToWhisperStrategy(Instance::InitParams::SamplingStrategy strategy) {
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
}

Instance::Instance(Model& model, InitParams)
    : m_model(model)
{}

Instance::~Instance() = default;

void Instance::runOp(std::string_view op,
    std::span<float> pcmf32,
    std::function<void(std::string)> resultCb) {
    if (op == "transcribe") {
        std::string res = runInference(pcmf32);
        resultCb(res);
    }
}

std::string Instance::runInference(std::span<float> pcmf32) {
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

    // default value means that temperature fallback will not be used
    float temperature_inc = 0.0f;

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

    //grammar_parser::parse_state grammar_parsed;
    } params;
    // run the inference
        {
            whisper_full_params wparams = whisper_full_default_params(ACToWhisperStrategy(m_params.samplingStrategy));

            //const bool use_grammar = (!params.grammar_parsed.rules.empty() && !params.grammar_rule.empty());
            wparams.strategy = WHISPER_SAMPLING_GREEDY;//(params.beam_size > 1 || use_grammar) ? WHISPER_SAMPLING_BEAM_SEARCH : WHISPER_SAMPLING_GREEDY;

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

            //wparams.debug_mode       = params.debug_mode;

            wparams.tdrz_enable      = params.tinydiarize; // [TDRZ]

            wparams.initial_prompt   = params.prompt.c_str();

            wparams.greedy.best_of        = params.best_of;
            wparams.beam_search.beam_size = params.beam_size;

            wparams.temperature_inc  = params.temperature_inc;
            wparams.temperature      = params.temperature;

            wparams.entropy_thold    = params.entropy_thold;
            wparams.logprob_thold    = params.logprob_thold;

            wparams.no_timestamps    = params.no_timestamps;

            //whisper_print_user_data user_data = { &params, &pcmf32s, 0 };

            //const auto & grammar_parsed = params.grammar_parsed;
            //auto grammar_rules = grammar_parsed.c_rules();

            // if (use_grammar) {
            //     if (grammar_parsed.symbol_ids.find(params.grammar_rule) == grammar_parsed.symbol_ids.end()) {
            //         fprintf(stderr, "%s: warning: grammar rule '%s' not found - skipping grammar sampling\n", __func__, params.grammar_rule.c_str());
            //     } else {
            //         wparams.grammar_rules = grammar_rules.data();
            //         wparams.n_grammar_rules = grammar_rules.size();
            //         wparams.i_start_rule = grammar_parsed.symbol_ids.at(params.grammar_rule);
            //         wparams.grammar_penalty = params.grammar_penalty;
            //     }
            // }

            // this callback is called on each new segment
            // if (!wparams.print_realtime) {
            //     wparams.new_segment_callback           = whisper_print_segment_callback;
            //     wparams.new_segment_callback_user_data = &user_data;
            // }

            // if (wparams.print_progress) {
            //     wparams.progress_callback           = whisper_print_progress_callback;
            //     wparams.progress_callback_user_data = &user_data;
            // }

            // examples for abort mechanism
            // in examples below, we do not abort the processing, but we could if the flag is set to true

            // the callback is called before every encoder run - if it returns false, the processing is aborted
            {
                static bool is_aborted = false; // NOTE: this should be atomic to avoid data race

                wparams.encoder_begin_callback = [](struct whisper_context * /*ctx*/, struct whisper_state * /*state*/, void * user_data) {
                    bool is_aborted = *(bool*)user_data;
                    return !is_aborted;
                };
                wparams.encoder_begin_callback_user_data = &is_aborted;
            }

            // the callback is called before every computation - if it returns true, the computation is aborted
            {
                static bool is_aborted = false; // NOTE: this should be atomic to avoid data race

                wparams.abort_callback = [](void * user_data) {
                    bool is_aborted = *(bool*)user_data;
                    return is_aborted;
                };
                wparams.abort_callback_user_data = &is_aborted;
            }

            if (whisper_full_parallel(m_model.context(), wparams, pcmf32.data(), pcmf32.size(), params.n_processors) != 0) {
                fprintf(stderr, "failed to process audio!\n");
                return "";
            }
        }

    std::string result;
    const int n_segments = whisper_full_n_segments(m_model.context());
    for (int i = 0; i < n_segments; ++i) {
        const char * text = whisper_full_get_segment_text(m_model.context(), i);
        std::string speaker = "";

        result += speaker + text + "\n";
    }

    return result;
}

} // namespace ac::whisper
