// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "JobSession.hpp"
#include "mem_ext.hpp"

struct llama_context;

namespace ac::llama {
class Model;

class AC_LLAMA_EXPORT Job {
public:
    struct InitParams {
        uint32_t ctxSize = 0; // context size for the model (0 = maximum allowed by model)
        uint32_t batchSize = 2048; // logical batch size for prompt processing (may be silently truncated to ctxSize)
        uint32_t ubatchSize = 0; // physical batch size for prompt processing (0 = batchSize)
    };

    explicit Job(Model& model, InitParams params);
    ~Job();

    // do an empty model run to load model data in cache
    void warmup();

    struct SessionParams {
        bool conversation = false;

        int numTokensToPredict = -1;

        uint32_t gaFactor = 1; // group-attention factor
        uint32_t gaWidth = 512; // group-attention width

        // if true, the inference tries to extend the context by truncating previous tokens
        // only used if gaFactor == 1
        bool infiniteContext = true;
    };

    // only one session per job can be active at a time
    JobSession newSession(std::string initialPrompt, const SessionParams params);

private:
    Model& m_model;
    astl::c_unique_ptr<llama_context> m_lctx;

    bool m_hasActiveSession = false;
};

} // namespace ac::llama
