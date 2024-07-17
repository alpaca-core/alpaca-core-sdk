// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Job.hpp"
#include "Model.hpp"
#include "Logging.hpp"
#include "ChatFormat.hpp"
#include "Vocab.hpp"
#include "iile.h"
#include "throw_ex.hpp"
#include <llama.h>
#include <cassert>


namespace ac::llama {

namespace {
llama_context_params llamaFromJobInitParams(const Job::InitParams& params) {
    llama_context_params llamaParams = llama_context_default_params();
    return llamaParams;
}
} // namespace

Job::Job(Model& model, InitParams params)
    : m_model(model)
    , m_lctx(llama_new_context_with_model(model.lmodel(), llamaFromJobInitParams(params)), llama_free)
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

itlib::generator<std::string> Job::run(RunParams params) {
    Vocab vocab(m_model);

    auto lctx = m_lctx.get();

    ChatFormat fmt(m_model.getChatTemplateId());
    std::vector<ChatMsg> chat;
    auto chatAddAndFormat = [&](std::string role, std::string text) {
        ChatMsg newMsg = {std::move(role), std::move(text)};
        auto ret = fmt.formatMsg(newMsg, chat, newMsg.role == "user");
        chat.push_back(std::move(newMsg));
        return ret;
    };

    std::vector<llama_token> inputTokens;
    if (!params.prompt.empty()) {
        if (params.conversation) {
            auto fmtChat = chatAddAndFormat("system", params.prompt);
            inputTokens = vocab.tokenize(fmtChat, true, true);
        }
        else {
            inputTokens = vocab.tokenize(params.prompt, true, true);
        }
    }
    else {
        // Should not run without any tokens
        inputTokens.push_back(llama_token_bos(m_model.lmodel()));
    }

    const auto ctxLen = llama_n_ctx(m_lctx.get());
    if (int(inputTokens.size()) > ctxLen - 4) {
        throw_ex{} << "Input too long. Got " << inputTokens.size() << " tokens, max: " << ctxLen - 4;
    }

    const auto numKeepTokens = m_model.shouldAddBosToken() + iile([&]() {
        if (params.conversation) return 0; // don't keep beginning of convo
        return int32_t(inputTokens.size());
    });

    // fix invariants
    if (params.conversation) {
        params.interactiveFirst = true;
    }
    if (params.interactiveFirst) {
        params.interactive = true;
    }

    bool interacting = params.interactive && params.interactiveFirst;

    co_yield "foo";
}

} // namespace ac::llama
