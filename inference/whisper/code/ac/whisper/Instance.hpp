// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"

#include <functional>
#include <string>
#include <span>

using WhisperCb = std::function<void(struct whisper_context*, struct whisper_state*,int, void*)>;

namespace ac::whisper {
class Model;

class AC_WHISPER_EXPORT Instance {
public:
    struct InitParams {
        // Text segment callback
        // Called on every newly generated text segment
        WhisperCb newSegmentCb;
        WhisperCb progressCb;

        enum SamplingStrategy {
            GREEDY,      // similar to OpenAI's GreedyDecoder
            BEAM_SEARCH, // similar to OpenAI's BeamSearchDecoder
        };
        SamplingStrategy samplingStrategy = GREEDY;
    };

    explicit Instance(Model& model, InitParams params);
    ~Instance();

    void runOp(std::string_view op, std::span<float> pcmf32, std::function<void(std::string)> resultCb);

private:
    std::string runInference(std::span<float> pcmf32);

    Model& m_model;
    InitParams m_params;
};

} // namespace ac::whisper
