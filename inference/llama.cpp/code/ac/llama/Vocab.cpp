// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Vocab.hpp"
#include "Model.hpp"
#include <llama.h>
#include <type_traits>
#include <cassert>

namespace ac::llama {

static_assert(std::is_same_v<Token, llama_token>);

Vocab::Vocab(const Model& model) : m_model(model) {}
Vocab::~Vocab() = default;

Token Vocab::decoderStartToken() const noexcept {
    auto model = m_model.lmodel();
    auto ret = llama_model_decoder_start_token(model);
    if (ret != -1) return ret;
    return llama_token_bos(model);
}

bool Vocab::isEog(Token token) const noexcept {
    return llama_token_is_eog(m_model.lmodel(), token);
}

std::vector<Token> Vocab::tokenize(std::string_view text, bool addSpecial, bool parseSpecial) const {
    int32_t numTokens = int32_t(text.length()) + 2 * addSpecial; // optimistic max
    std::vector<Token> ret(numTokens);
    numTokens = llama_tokenize(m_model.lmodel(), text.data(), int32_t(text.length()), ret.data(), numTokens, addSpecial, parseSpecial);
    if (numTokens < 0) {
        ret.resize(-numTokens);
        [[maybe_unused]] int check =
            llama_tokenize(m_model.lmodel(), text.data(), int32_t(text.length()), ret.data(), -numTokens, addSpecial, parseSpecial);
        assert(check == -numTokens);
    }
    else {
        ret.resize(numTokens);
    }
    return ret;
}

} // namespace ac::llama
