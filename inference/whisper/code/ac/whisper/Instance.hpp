// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <astl/mem_ext.hpp>

#include <functional>
#include <string>

namespace ac::whisper {
class Model;

class AC_WHISPER_EXPORT Instance {
public:
    struct InitParams {
        uint32_t ctxSize = 0; // context size for the model (0 = maximum allowed by model)
        uint32_t batchSize = 2048; // logical batch size for prompt processing (may be silently truncated to ctxSize)
        uint32_t ubatchSize = 0; // physical batch size for prompt processing (0 = batchSize)
    };

    explicit Instance(Model& model, InitParams params);
    ~Instance();

    void runOp(std::string_view op, const std::vector<float>& pcmf32, const std::vector<std::vector<float>>& pcmf32s, std::function<void(std::string)> resultCb);

private:
    std::string runInference(const std::vector<float>& pcmf32, const std::vector<std::vector<float>>& pcmf32s);

    Model& m_model;
};

} // namespace ac::whisper
