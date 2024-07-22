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

    ac::llama::Model model("D:/mod/Mistral-7B-Instruct-v0.1-GGUF/mistral-7b-instruct-v0.1.Q8_0.gguf");
    //ac::llama::Model model("D:/mod/gpt2/gguf/gpt2.Q6_K.gguf");
    //ac::llama::Model model("D:/mod/gpt2/gguf-gpt2-chatbot/gpt2-chatbot.Q8_0.gguf");

    ac::llama::Job job(model);

    job.warmup();

    job.decode("A chat between a user and a helpful AI assistant", {.conversation=true});
    job.decode("How to get to the moon?", {.conversation=true});

    for (auto s : job.generate()) {
        std::cout << model.vocab().tokenToString(s);
    }

    std::cout << std::endl;

    job.decode("Can I join NASA?", { .conversation = true });

    for (auto s : job.generate()) {
        std::cout << model.vocab().tokenToString(s);
    }

    //std::cout << std::endl;

    return 0;
}
