// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "mem_ext.hpp"
#include <string>

struct llama_model;
struct llama_model_params;

namespace ac::llama {
class Job;

class AC_LLAMA_EXPORT Model {
public:
    struct Params {
    };

    explicit Model(const char* pathToGguf, Params params = {});
    ~Model();

    uint32_t trainCtxLength() const noexcept;
    bool shouldAddBosToken() const noexcept;

    // fallback to "chatml" if the uderlying model does not provide a chat template
    std::string getChatTemplateId() const;
private:
    friend Job;
    astl::c_unique_ptr<llama_model> m_lmodel;
};
} // namespace ac::llama
