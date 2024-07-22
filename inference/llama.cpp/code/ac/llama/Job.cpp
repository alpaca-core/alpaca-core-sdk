// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Job.hpp"
#include "Model.hpp"
#include "Logging.hpp"
#include "ChatFormat.hpp"
#include "Sampler.hpp"
#include "iile.h"
#include "throw_ex.hpp"
#include <llama.h>
#include <cassert>


namespace ac::llama {

namespace {
llama_context_params llamaFromJobInitParams(const Job::InitParams& params) {
    llama_context_params llamaParams = llama_context_default_params();
    llamaParams.n_batch = params.batchSize;
    return llamaParams;
}
} // namespace

Job::Job(Model& model, InitParams params)
    : m_model(model)
    , m_lctx(llama_new_context_with_model(model.lmodel(), llamaFromJobInitParams(params)), llama_free)
    , m_chatFmt(model.getChatTemplateId())
{
    if (!m_lctx) {
        throw_ex{} << "Failed to create llama context";
    }
    assert(model.lmodel() == llama_get_model(m_lctx.get()));

    const auto ctxLen = llama_n_ctx(m_lctx.get());
    const auto ctxTrain = model.trainCtxLength();
    if (ctxLen > ctxTrain) {
        LLAMA_LOG(Warning, "Job requested context length ", ctxLen, " is greater than the model's training context length ", ctxTrain);
    }
}

Job::~Job() = default;

void Job::warmup() {
    LLAMA_LOG(Info, "Running warmup");

    auto lctx = m_lctx.get();
    auto model = m_model.lmodel();

    std::vector<llama_token> tmp;
    llama_token bos = llama_token_bos(model);
    llama_token eos = llama_token_eos(model);
    // some models (e.g. T5) don't have a BOS token
    if (bos != -1) {
        tmp.push_back(bos);
    }
    tmp.push_back(eos);

    const auto ntokens = int32_t(tmp.size());

    if (llama_model_has_encoder(model)) {
        llama_encode(lctx, llama_batch_get_one(tmp.data(), ntokens, 0, 0));
        llama_token decoder_start_token_id = llama_model_decoder_start_token(model);
        if (decoder_start_token_id == -1) {
            decoder_start_token_id = bos;
        }
        tmp.clear();
        tmp.push_back(decoder_start_token_id);
    }
    llama_decode(lctx, llama_batch_get_one(tmp.data(), ntokens, 0, 0));
    llama_kv_cache_clear(lctx);
    llama_synchronize(lctx);
    llama_reset_timings(lctx);
}

void Job::tryExpandContext(std::span<const Token> tokens) {
    auto lctx = m_lctx.get();

    const auto ctxLen = llama_n_ctx(lctx);
    const auto numKeep = m_sessionData.numKeep;

    const auto gaFactor = m_sessionData.params.gaFactor;

    auto& numPast = m_sessionData.numPast;

    const auto maxTokens = ctxLen - 4; // ref #16

    // Ensure the input doesn't exceed the context size by truncating embd if necessary.
    if (tokens.size() > maxTokens) {
        const auto skipped = tokens.size() - maxTokens;
        tokens = tokens.first(maxTokens);
        LLAMA_LOG(Warning, "Input too long. Skipping ", skipped, " tokens");
    }

    bool haveFullContextMitigation = false;
    if (gaFactor == 1) {
        // infinite text generation via context shifting
        // if we run out of context:
        // - take the n_keep first tokens from the original prompt (via numPast)
        // - take half of the last (n_ctx - n_keep) tokens and recompute the logits in batches
        const auto num = numPast + tokens.size();
        if (num >= ctxLen) {
            if (!m_sessionData.params.infiniteContext) {
                throw_ex{} << "context limit of " << ctxLen << " reached";
            }

            const auto numLeft = numPast - numKeep;
            const int numDiscard = numLeft / 2; // somewhat arbitrary

            LLAMA_LOG(Debug, "Context is full. Swapping: past = ", numPast, ", numLeft: ", numLeft,
                ", ctxLen: ", ctxLen, ", numKeep: ", numKeep, ", numDiscard: ", numDiscard);

            llama_kv_cache_seq_rm(lctx, 0, numKeep, numKeep + numDiscard);
            llama_kv_cache_seq_add(lctx, 0, numKeep + numDiscard, numPast, -numDiscard);

            numPast -= numDiscard;
            haveFullContextMitigation = true;
        }
    }
    else {
        const uint32_t gaWidth = m_sessionData.params.gaWidth;
        uint32_t& gaIndex = m_sessionData.gaIndex;

        while (numPast >= gaIndex + gaWidth) {
            // context extension via Self-Extend
            const int ib = (gaFactor * gaIndex) / gaWidth;
            const int bd = (gaWidth / gaFactor) * (gaFactor - 1);
            const int dd = (gaWidth / gaFactor) - ib * bd - gaWidth;

            LLAMA_LOG(Debug, "Group attention shift: ib = ", ib, ", bd = ", bd, ", dd = ", dd);

            llama_kv_cache_seq_add(lctx, 0, gaIndex, numPast, ib * bd);
            llama_kv_cache_seq_div(lctx, 0, gaIndex + ib * bd, gaIndex + ib * bd + gaWidth, gaFactor);
            llama_kv_cache_seq_add(lctx, 0, gaIndex + ib * bd + gaWidth, numPast + ib * bd, dd);

            numPast -= bd;

            gaIndex += gaWidth / gaFactor;
            haveFullContextMitigation = true;
        }
    }

    if (haveFullContextMitigation) {
        LLAMA_LOG(Info, "Context full mitigation performed: past = ", numPast, ", tokens = ", tokens.size());
    }
}

void Job::doDecode(std::span<Token> tokens) {
    // first try to expand the context if needed
    tryExpandContext(tokens);

    for (auto t : tokens) {
        m_sessionData.sampler.accept(t);
    }

    auto lctx = m_lctx.get();
    auto& numPast = m_sessionData.numPast;
    const auto batchSize = llama_n_batch(lctx);

    // decode with batches of batchSize
    while (!tokens.empty()) {
        auto batchTokens = tokens.size() > batchSize ? tokens.first(batchSize) : tokens;
        tokens = tokens.subspan(batchTokens.size());

        auto batch = llama_batch_get_one(batchTokens.data(), int(batchTokens.size()), numPast, 0);
        if (llama_decode(lctx, batch) != 0) {
            throw_ex{} << "Failed to decode tokens";
        }
        numPast += uint32_t(batchTokens.size());
    }
}

void Job::setup(std::string_view prompt, const RunParams& params) {
    auto lctx = m_lctx.get();

    if (m_sessionData.initialized) {
        m_sessionData.~SessionData();
        new (&m_sessionData) SessionData();

        llama_kv_cache_clear(lctx);
        llama_synchronize(lctx);
        llama_reset_timings(lctx);
    }
    m_sessionData.params = params;

    std::vector<llama_token> inputTokens;

    auto& vocab = m_model.vocab();

    if (prompt.empty()) {
        // Should not run without any tokens
        inputTokens.push_back(llama_token_bos(m_model.lmodel()));
    }
    else {
        if (params.conversation) {
            auto fmtChat = chatAddAndFormat("system", std::string(prompt));
            inputTokens = vocab.tokenize(fmtChat, true, true);
        }
        else {
            inputTokens = vocab.tokenize(prompt, true, true);
        }
    }

    const auto ctxLen = llama_n_ctx(lctx);
    if (inputTokens.size() > ctxLen - 4) {
        throw_ex{} << "Input too long. Got " << inputTokens.size() << " tokens, max: " << ctxLen - 4;
    }

    m_sessionData.numKeep = int32_t(inputTokens.size());

    if (params.gaFactor != 1) {
        const uint32_t gaFactor = params.gaFactor;
        const uint32_t gaWidth = params.gaWidth;
        if (gaWidth % gaFactor != 0) {
            throw_ex{} << "Group-attention width " << gaWidth << " must be a multiple of group-attention factor " << gaFactor;
        }
        LLAMA_LOG(Info, "self-extend: train = ", m_model.trainCtxLength(), ", gaFactor = ", gaFactor, ", gaWidth = ", gaWidth);
    }

    if (m_model.hasEncoder()) {
        auto batch = llama_batch_get_one(inputTokens.data(), int32_t(inputTokens.size()), 0, 0);
        auto res = llama_encode(lctx, batch);
        if (res != 0) {
            throw_ex{} << "Failed to encode input";
        }
        inputTokens.clear();
        inputTokens.push_back(vocab.decoderStartToken());
    }

    m_sessionData.initialized = true;

    doDecode(inputTokens);
}

void Job::decode(std::string_view prompt) {
    if (!m_sessionData.initialized) {
        throw_ex{} << "Job hasn't been set up";
    }

    auto& vocab = m_model.vocab();
    std::vector<Token> promptTokens;

    if (m_sessionData.params.conversation) {
        auto fmtChat = chatAddAndFormat("user", std::string(prompt));
        promptTokens = vocab.tokenize(fmtChat, false, true);
    }
    else {
        promptTokens = vocab.tokenize(prompt, false, false);
    }

    // reset sampling and don't allow previous inputs to affect the generation
    m_sessionData.sampler.reset();

    doDecode(promptTokens);
}

std::string Job::chatAddAndFormat(std::string role, std::string text) {
    ChatMsg newMsg = { std::move(role), std::move(text) };
    auto ret = m_chatFmt.formatMsg(newMsg, m_sessionData.chat, newMsg.role == "user");
    m_sessionData.chat.push_back(std::move(newMsg));
    return ret;
}

itlib::generator<Token> Job::generate(uint32_t maxTokens) {
    auto lctx = m_lctx.get();
    auto& sampler = m_sessionData.sampler;
    auto& vocab = m_model.vocab();

    while (maxTokens) {
        auto token = sampler.sample(lctx);
        co_yield token;

        if (vocab.isEog(token)) {
            break;
        }

        sampler.accept(token);
        doDecode({&token, 1});
        --maxTokens;
    }

    if (m_sessionData.params.conversation) {
        // so here we add just some random message to the conversation to keep the flow of the conversation
        // it is not going to be used otherwise
        chatAddAndFormat("assistant", "msg");
    }
}

} // namespace ac::llama
