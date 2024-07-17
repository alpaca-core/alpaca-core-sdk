// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <iostream>

#include <ac/llama/Init.hpp>
#include <ac/llama/Model.hpp>
#include <ac/llama/ChatFormat.hpp>
#include <ac/llama/Job.hpp>

#include <jalog/Instance.hpp>
#include <jalog/sinks/ColorSink.hpp>

#include <vector>

int main() {
    jalog::Instance jl;
    jl.setup().add<jalog::sinks::ColorSink>();

    ac::llama::initLibrary();

    ac::llama::Model model("D:/mod/Mistral-7B-v0.1-GGUF/mistral-7b-v0.1.Q6_K.gguf");

    ac::llama::Job job(model);

    job.warmup();

    for (auto s : job.run({})) {
        std::cout << s << "\n";
    }

    return 0;
}
