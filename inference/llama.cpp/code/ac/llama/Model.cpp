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
    : m_lmodel(llama_load_model_from_file(pathToGguf, llamaFromModelParams(params)), llama_free_model)
{
    if (!m_lmodel) {
        throw std::runtime_error("Failed to load model");
    }
}

Model::~Model() = default;


uint32_t Model::trainCtxLength() const noexcept {
    return uint32_t(llama_n_ctx_train(m_lmodel.get()));
}

bool Model::shouldAddBosToken() const noexcept {
    const int lbos = llama_add_bos_token(m_lmodel.get());
    if (lbos != -1) return !!lbos;

    return llama_vocab_type(m_lmodel.get()) == LLAMA_VOCAB_TYPE_SPM;
}

bool Model::hasEncoder() const noexcept {
    return llama_model_has_encoder(m_lmodel.get());
}

std::string Model::getChatTemplateId() const {
    // load template from model
    constexpr size_t bufSize = 2048; // longest known template is about 1200 bytes
    std::unique_ptr<char[]> tplBuf(new char[bufSize]);

    const char* key = "tokenizer.chat_template";

    int32_t len = llama_model_meta_val_str(m_lmodel.get(), key, tplBuf.get(), bufSize);
    if (len < 0) {
        return "chatml"; // default fallback
    }

    return std::string(tplBuf.get(), len);
}

} // namespace ac::llama
