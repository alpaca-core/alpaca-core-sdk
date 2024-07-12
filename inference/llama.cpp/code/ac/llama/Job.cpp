// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Job.hpp"
#include "Model.hpp"
#include "Logging.hpp"
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

} // namespace ac::llama
