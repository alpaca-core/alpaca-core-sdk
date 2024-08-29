// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"

#include <astl/mem_ext.hpp>

#include <functional>
#include <string>
#include <span>

struct whisper_context;

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

    explicit Instance(Model& model, InitParams params);
    ~Instance();

    void runOp(std::string_view op, std::span<float> pcmf32, std::function<void(std::string)> resultCb);

    whisper_context* context() const noexcept { return m_ctx.get(); }

private:
    std::string runInference(std::span<float> pcmf32);

    Model& m_model;
    InitParams m_params;
    astl::c_unique_ptr<whisper_context> m_ctx;
};

} // namespace ac::whisper
