// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Token.hpp"
#include <itlib/flat_map.hpp>
#include <vector>
#include <random>

struct llama_token_data;
struct llama_context;
struct llama_token_data_array;

namespace ac::llama {

class AC_LLAMA_EXPORT Sampler {
public:
    enum class SamplingType {
        Top_K,
        Top_P,
        Min_P,
        Tfs_Z,
        Typical_P,
        Temperature
    };

    struct Params {
        uint32_t numPrev = 64; // number of previous tokens to remember
        uint32_t rngSeed = 0; // seed for the random number generator

        int32_t topK = 40;       // <= 0 to use vocab size
        float topP = 0.95f;      // 1.0 = disabled
        float minP = 0.05f;      // 0.0 = disabled
        float tfsZ = 1.00f;      // 1.0 = disabled
        float typicalP = 1.00f;  // 1.0 = disabled
        float temp = 0.80f;      // <= 0.0 to sample greedily, 0.0 to not output probabilities
        float tempRange = 0.00f; // 0.0 = disabled
        float tempExp = 1.00f;   // controls how entropy maps to temperature in dynamic temperature sampler

        struct RepetitionPenalty {
            int32_t numTokens = 64;    // last n tokens to penalize (0 = disable penalty, -1 = context size)
            float   repeat    = 1.00f; // 1.0 = disabled
            float   freq      = 0.00f; // 0.0 = disabled
            float   present   = 0.00f; // 0.0 = disabled
        } repetitionPenalty;

        struct Mirostat {
            int32_t ver = 0; // 0 = disabled, 1 = mirostat, 2 = mirostat 2.0
            float tau = 5.00f; // target entropy
            float eta = 0.10f; // learning rate
        } mirostat;

        bool penalizeNewline = false; // consider newlines as a repeatable token

        std::vector<SamplingType> samplerSequence = {
            SamplingType::Top_K,
            SamplingType::Tfs_Z,
            SamplingType::Typical_P,
            SamplingType::Top_P,
            SamplingType::Min_P,
            SamplingType::Temperature
        };

        // Classifier-Free Guidance
        // https://arxiv.org/abs/2306.17806
        struct Cfg {
            std::string negativePrompt; // string to help guidance
            float       scale = 1.f; // how strong is guidance
        } cfg;

        itlib::flat_map<Token, float> logitBias; // bias for specific tokens

        std::vector<Token> penaltyPromptTokens;
    };

    Sampler(Params params);
    ~Sampler();

    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;

    // reset the sampler state
    void reset();

    void reseedRng(uint32_t seed) noexcept { m_rng.seed(seed); }

    Token last() const noexcept { return m_prev.back(); }

    // sample a token from the current distribution
    // cfgCtx is optional in case classifier-free guidance is used
    // idx is optional for sampling from the logits of the ith token
    Token sample(llama_context* lctx, llama_context* cfgCtx = nullptr, int idx = -1);

    // accept token as sampled (add to prev)
    void accept(Token id);

private:
    const Params m_params;

    llama_token_data_array llama_sampling_prepare(llama_context* lctx, llama_context* cfgCtx, int idx);

    // previously sampled tokens (ring buffer but kept as vector because of llama_sample_repetition_penalties)
    // used for repetition penalties and antiprompts
    std::vector<Token> m_prev;

    std::vector<llama_token_data> m_cur; // current tokens for sampling (one for each vocabulary entry)

    size_t m_numValidTokens = 0; // number of correct top tokens with correct probs

    float m_mirostatMu; // optionally used for mirostat sampling

    std::minstd_rand m_rng;

    // TODO: grammar-based sampling (#14)
};

} // namespace ac::llama
