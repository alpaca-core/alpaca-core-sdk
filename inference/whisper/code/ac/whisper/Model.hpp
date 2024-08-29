// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"

#include <astl/mem_ext.hpp>

#include <string>

struct whisper_context;

namespace ac::whisper {
class Job;

class AC_WHISPER_EXPORT Model {
public:
    struct Params {
        bool gpu = true; // try to load data on gpu
    };

    explicit Model(const char* pathToBin, Params params);
    ~Model();

    const Params& params() const noexcept { return m_params; }

    whisper_context* context() const noexcept { return m_ctx.get(); }

private:
    const Params m_params;
    astl::c_unique_ptr<whisper_context> m_ctx;
};
} // namespace ac::whisper
