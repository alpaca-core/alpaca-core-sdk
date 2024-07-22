// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Token.hpp"
#include "Sampler.hpp"
#include "ChatFormat.hpp"
#include "mem_ext.hpp"
#include <itlib/generator.hpp>
#include <string>
#include <span>

struct llama_context;

namespace ac::llama {
class Model;

class AC_LLAMA_EXPORT Job {
public:
    struct InitParams {
        uint32_t batchSize = 2048; // logical batch size for prompt processing (must be >=32 to use BLAS)
    };

    explicit Job(Model& model, InitParams params = {});
    ~Job();

    // do an empty model run to load model data in cache
    void warmup();

    struct RunParams {
        bool conversation = false;

        int numTokensToPredict = -1;

        uint32_t gaFactor = 1; // group-attention factor
        uint32_t gaWidth = 512; // group-attention width

        // if true, the inference tries to extend the context by truncating previous tokens
        // only used if gaFactor == 1
        bool infiniteContext = true;
    };

    void setup(std::string_view prompt, const RunParams& params);
    void decode(std::string_view prompt);
    itlib::generator<Token> generate(uint32_t maxTokens = uint32_t(-1));

private:
    Model& m_model;
    astl::c_unique_ptr<llama_context> m_lctx;

    void tryExpandContext(std::span<const Token> tokens); // try to expand context to accommodate tokens

    // we should have a `const Token` span but llama_batch doesn't let us
    void doDecode(std::span<Token> tokens);

    std::string chatAddAndFormat(std::string role, std::string text);

    ChatFormat m_chatFmt;

    struct SessionData {
        RunParams params;
        Sampler sampler;
        std::vector<ChatMsg> chat;
        uint32_t numKeep = 0; // number of tokens to keep in the context in case we overflow
        uint32_t numPast = 0; // number of tokens in the context (that's prompts + generated)

        // group attention state
        uint32_t gaIndex = 0; // number of grouped KV tokens (only used if params.gaFactor > 1)

        bool initialized = false;
    };
    SessionData m_sessionData;
};

} // namespace ac::llama
