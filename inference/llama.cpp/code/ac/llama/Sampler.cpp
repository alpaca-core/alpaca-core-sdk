// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Sampler.hpp"
#include <llama-sampling.h>
#include <itlib/qalgorithm.hpp>
#include <itlib/stride_span.hpp>
#include <span>

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

llama_token_data_array Sampler::prepareSampling(llama_context* lctx, llama_context* cfgCtx, int idx) {
    auto model = llama_get_model(lctx);
    const auto vocabSize = llama_n_vocab(model);

    auto logits = llama_get_logits_ith(lctx, idx);

    // apply bias if any
    for (const auto& [token, bias] : m_params.logitBias) {
        logits[token] += bias;
    }

    if (cfgCtx) {
        auto guidanceLogits = llama_get_logits_ith(cfgCtx, idx);
        llama_sample_apply_guidance(lctx, logits, guidanceLogits, m_params.cfg.scale);
    }

    m_cur.resize(vocabSize);
    for (Token t = 0; t < vocabSize; ++t) {
        m_cur[t] = {t, logits[t], 0};
    }

    llama_token_data_array curAr = {m_cur.data(), m_cur.size(), false};

    if (m_params.repetitionPenalty.numTokens) {
        // apply penalties

        std::span<const Token> penaltyTokens;
        if (m_params.penaltyPromptTokens.empty()) {
            penaltyTokens = m_prev;
        }
        else {
            penaltyTokens = m_params.penaltyPromptTokens;
        }

        if (m_params.repetitionPenalty.numTokens > 0 && m_params.repetitionPenalty.numTokens < int(penaltyTokens.size())) {
            // take last n tokens
            penaltyTokens = penaltyTokens.subspan(penaltyTokens.size() - m_params.repetitionPenalty.numTokens);
        }

        // store nl logit
        const auto nl = llama_token_nl(model);
        const float nlLogit = logits[nl];

        auto& rep = m_params.repetitionPenalty;
        llama_sample_repetition_penalties(lctx, &curAr,
            penaltyTokens.data(), penaltyTokens.size(),
            rep.repeat, rep.freq, rep.present);

        if (!m_params.penalizeNewline) {
            // restore nl
            // linear search since the previous call would have reordered the logits
            std::span<llama_token_data> cur(curAr.data, curAr.size);
            if (auto p = itlib::pfind_if(cur, [nl](const auto& t) { return t.id == nl; })) {
                p->logit = nlLogit;
            }
        }
    }

    return curAr;
}

Token Sampler::sampleImpl(llama_context* lctx, llama_context* cfgCtx, int idx, bool /*resample*/) {
    auto curAr = prepareSampling(lctx, cfgCtx, idx);

    Token ret = 0;

    const auto temp = m_params.temp;
    const auto& miro = m_params.mirostat;

    if (temp < 0) {
        // greedy sampling with probs
        llama_sample_softmax(lctx, &curAr);
        ret = curAr.data[0].id;
    }
    else if (temp == 0) {
        // greedy sampling without probs
        ret = llama_sample_token_greedy(lctx, &curAr);
    }
    else if (miro.ver == 1) {
        // sample with temperature
        const int miroM = 100;
        llama_sample_temp(lctx, &curAr, temp);
        ret = llama_sample_token_mirostat(lctx, &curAr, miro.tau, miro.eta, miroM, &m_mirostatMu);
    }
    else if (miro.ver == 2) {
        llama_sample_temp(lctx, &curAr, temp);
        ret = llama_sample_token_mirostat_v2(lctx, &curAr, miro.tau, miro.eta, &m_mirostatMu);
    }
    else {
        // sequence sampling

        const size_t minKeep = 1; // ref #15
        for (auto type : m_params.samplerSequence) {
            switch (type)
            {
            case SamplingType::Top_K: llama_sample_top_k(lctx, &curAr, m_params.topK, minKeep); break;
            case SamplingType::Tfs_Z: llama_sample_tail_free(lctx, &curAr, m_params.tfsZ, minKeep); break;
            case SamplingType::Typical_P: llama_sample_typical(lctx, &curAr, m_params.typicalP, minKeep); break;
            case SamplingType::Top_P: llama_sample_top_p(lctx, &curAr, m_params.topP, minKeep); break;
            case SamplingType::Min_P: llama_sample_min_p(lctx, &curAr, m_params.minP, minKeep); break;
            case SamplingType::Temperature:
                if (m_params.tempRange > 0) {
                    const float min = std::max(0.f, temp - m_params.tempRange);
                    const float max = std::min(0.f, temp + m_params.tempRange);
                    llama_sample_entropy(lctx, &curAr, min, max, m_params.tempExp);
                }
                else {
                    llama_sample_temp(lctx, &curAr, temp);
                }
                break;
            default: break;
            }
        }

        // sample with rng
        llama_sample_softmax(lctx, &curAr);

        auto probs = itlib::make_stride_span_member_view(curAr.data, curAr.size, &llama_token_data::p);
        std::discrete_distribution dist(probs.begin(), probs.end());
        int s = dist(m_rng);
        ret = curAr.data[s].id;
    }

    m_numValidTokens = temp == 0 ? 0 : curAr.size;

    return ret;
}

Token Sampler::sample(llama_context* lctx, llama_context* cfgCtx, int idx) {
    return sampleImpl(lctx, cfgCtx, idx, false);
}

std::vector<Token> Sampler::prevTokens(uint32_t n) const noexcept {
    if (m_prev.size() < size_t(n)) {
        return m_prev;
    }
    return std::vector<Token>(m_prev.begin(), m_prev.begin() + n);
}

void Sampler::reset() {
    m_prev.clear();
    m_prev.push_back(0);
    m_cur.clear();
    m_numValidTokens = 0;
}

} // namespace ac::llama
