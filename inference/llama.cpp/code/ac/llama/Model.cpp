// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Model.hpp"
#include <llama.h>
#include <stdexcept>

namespace ac::llama {

namespace {
llama_model_params llamaFromModelParams(const Model::Params& params)
{
    llama_model_params llamaParams = llama_model_default_params();
    return llamaParams;
}
} // namespace

Model::Model(const char* pathToGguf, Params params)
    : m_model(llama_load_model_from_file(pathToGguf, llamaFromModelParams(params)), llama_free_model)
{
    if (!m_model) {
        throw std::runtime_error("Failed to load model");
    }
}

Model::~Model() = default;


} // namespace ac::llama
