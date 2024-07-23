// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "mem_ext.hpp"
#include "Vocab.hpp"
#include <string>

struct llama_model;
struct llama_model_params;

namespace ac::llama {
class Job;

class AC_LLAMA_EXPORT Model {
public:
    struct Params {
        bool gpu = true; // try to load data on gpu
        bool vocabOnly = false; // do not load model, only vocab
    };

    explicit Model(const char* pathToGguf, Params params = {});
    ~Model();

    uint32_t trainCtxLength() const noexcept;
    bool shouldAddBosToken() const noexcept;
    bool hasEncoder() const noexcept;

    // fallback to "chatml" if the uderlying model does not provide a chat template
    std::string getChatTemplateId() const;

    llama_model* lmodel() noexcept { return m_lmodel.get(); }
    const llama_model* lmodel() const noexcept { return m_lmodel.get(); }

    const Vocab& vocab() const noexcept { return m_vocab; }
private:
    astl::c_unique_ptr<llama_model> m_lmodel;

    Vocab m_vocab{*this};
};
} // namespace ac::llama
