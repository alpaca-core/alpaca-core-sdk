// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Token.hpp"
#include "mem_ext.hpp"
#include <itlib/generator.hpp>
#include <string>

struct llama_context;

namespace ac::llama {
class Model;

class AC_LLAMA_EXPORT Job {
public:
    struct InitParams {
    };

    explicit Job(Model& model, InitParams params = {});
    ~Job();

    // do an empty model run to load model data in cache
    void warmup();

    struct RunParams {
        std::string prompt;

        bool conversation = false;
        bool interactiveFirst = false;

        int numTokensToPredict = -1;

        uint32_t gaFactor = 1; // group-attention factor
        uint32_t gaWidth = 512; // group-attention width

        uint32_t batchSize = 2048; // logical batch size for prompt processing (must be >=32 to use BLAS)
    };

    itlib::generator<Token> run(RunParams rp);

private:
    Model& m_model;
    astl::c_unique_ptr<llama_context> m_lctx;
};

} // namespace ac::llama
