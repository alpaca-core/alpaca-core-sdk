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
}

Job::~Job() = default;

} // namespace ac::llama
