// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "mem_ext.hpp"

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
private:
    friend Job;
    astl::c_unique_ptr<llama_model> m_lmodel;
};
} // namespace ac::llama
