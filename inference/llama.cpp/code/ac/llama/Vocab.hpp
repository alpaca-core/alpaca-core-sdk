// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <vector>
#include <string_view>

namespace ac::llama {

class Model;

struct AC_LLAMA_EXPORT Vocab {
public:
    using Token = int32_t;

    Vocab(const Model& model);
    ~Vocab();

    std::vector<Token> tokenize(std::string_view text, bool addSpecial, bool parseSpecial) const;

private:
    const Model& m_model;
};

} // namespace ac::llama
