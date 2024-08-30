// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"

#include <astl/mem_ext.hpp>

#include <functional>
#include <string>
#include <span>

struct whisper_state;

namespace ac::whisper {
class Model;

class AC_WHISPER_EXPORT Instance {
public:
    struct InitParams {
        enum SamplingStrategy {
            GREEDY,      // similar to OpenAI's GreedyDecoder
            BEAM_SEARCH, // similar to OpenAI's BeamSearchDecoder
        };
        SamplingStrategy samplingStrategy = GREEDY;
    };

    Instance(Model& model, InitParams params);
    ~Instance();

    std::string transcribe(std::span<float> pcmf32);

private:
    std::string runInference(std::span<float> pcmf32);

    Model& m_model;
    InitParams m_params;
    astl::c_unique_ptr<whisper_state> m_state;
};

} // namespace ac::whisper
