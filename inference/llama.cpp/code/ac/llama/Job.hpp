// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
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
        bool interactive = false;
        bool interactiveFirst = false;
    };

    itlib::generator<std::string> run(RunParams rp);

private:
    Model& m_model;
    astl::c_unique_ptr<llama_context> m_lctx;
};

} // namespace ac::llama
