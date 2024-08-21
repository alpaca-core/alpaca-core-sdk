// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Model.hpp"
#include <whisper.h>
#include <astl/move.hpp>
#include <stdexcept>

namespace ac::whisper {

namespace {
whisper_context_params whisperFromModelParams(const Model::Params& params)
{
    whisper_context_params whisperParams = whisper_context_default_params();
    whisperParams.use_gpu = params.gpu;

    return whisperParams;
}
} // namespace

Model::Model(const char* pathToBin, Params params)
    : m_params(astl::move(params))
    , m_ctx(whisper_init_from_file_with_params(pathToBin, whisperFromModelParams(params)), whisper_free)
{
    if (!m_ctx) {
        throw std::runtime_error("Failed to load model");
    }
}

Model::~Model() = default;


} // namespace ac::whisper
