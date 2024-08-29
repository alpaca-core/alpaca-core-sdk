// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
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

    const std::string& pathToBin() const noexcept { return m_pathToBin; }
    const Params& params() const noexcept { return m_params; }

private:
    const Params m_params;
    std::string m_pathToBin;
};
} // namespace ac::whisper
