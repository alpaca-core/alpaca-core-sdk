// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Token.hpp"
#include <vector>

struct llama_token_data;

namespace ac::llama {

class AC_LLAMA_EXPORT Sampler {
public:
    struct Params {
    };

    Sampler(Params params);
    ~Sampler();

    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;

private:
    const Params m_params;
    std::vector<Token> m_prev; // previously sampled tokens
    std::vector<llama_token_data> m_cur; // current tokens for sampling
};

} // namespace ac::llama
