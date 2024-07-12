// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "mem_ext.hpp"
#include <vector>

struct llama_context;

namespace ac::llama {
class Model;

class Job {
public:
    struct Params {
    };

    explicit Job(Model& model, Params params = {});
    ~Job();
private:
    astl::c_unique_ptr<llama_context> m_lctx;

    using LlamaToken = int32_t;
    std::vector<LlamaToken> m_inputTokens;
};

} // namespace ac::llama
