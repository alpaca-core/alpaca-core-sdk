// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Job.hpp"
#include "Model.hpp"
#include "Logging.hpp"
#include "ChatMsg.hpp"
#include <llama.h>

namespace ac::llama {

namespace {
llama_context_params llamaFromJobParams(const Job::Params& params)
{
    llama_context_params llamaParams = llama_context_default_params();
    return llamaParams;
}
} // namespace

Job::Job(Model& model, Params params)
    : m_lctx(llama_new_context_with_model(model.m_lmodel.get(), llamaFromJobParams(params)), llama_free)
{
    static_assert(std::is_same_v<LlamaToken, llama_token>);

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
    auto model = llama_get_model(lctx);

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

} // namespace ac::llama
