// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Sampler.hpp"
#include <llama.h>

namespace ac::llama {

Sampler::Sampler(Params params)
    : m_params(params)
    , m_rng(params.rngSeed)
{
    if (m_params.numPrev == 0) {
        throw std::runtime_error("Sampler: numPrev must be > 0");
    }

    m_prev.reserve(m_params.numPrev);
    m_prev.push_back(0); // to we can have `last()` always work
}

Sampler::~Sampler() = default;

void Sampler::accept(Token id) {
    if (m_prev.size() == m_params.numPrev) {
        m_prev.erase(m_prev.begin());
    }
    m_prev.push_back(id);
}

llama_token_data_array Sampler::llama_sampling_prepare(llama_context* lctx, llama_context* cfgCtx, int idx) {
    const auto vocabSize = llama_n_vocab(llama_get_model(lctx));

    const auto penaltyNumTokens = m_params.repetitionPenalty.numTokens < 0 ? int(m_prev.size()) : m_params.repetitionPenalty.numTokens;
    const auto penaltyRepeat = m_params.repetitionPenalty.repeat;
    const auto penaltyFreq = m_params.repetitionPenalty.freq;
    const auto penaltyPresent = m_params.repetitionPenalty.present;
    const auto penalizeNl = m_params.penalizeNewline;

    auto logits = llama_get_logits_ith(lctx, idx);

    return {};
}

Token Sampler::sample(llama_context* lctx, llama_context* cfgCtx, int idx) {
    return {};
}

} // namespace ac::llama
