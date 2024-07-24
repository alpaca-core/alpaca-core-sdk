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

ac::llama::SessionCoroutine s() {
    while (true) {
        auto& str = co_await ac::llama::SessionCoroutine::Prompt{};
        if (str == "exit") {
            co_return;
        }
        co_yield ac::llama::Token(str.length());
    }
}

int main() {
    jalog::Instance jl;
    jl.setup().add<jalog::sinks::ColorSink>();

    ac::llama::initLibrary();

    auto coro = s();
    for (int i = 0; i < 10; ++i) {
        if (i == 2) {
            coro.setPrompt("Hello");
        }
        if (i == 5) {
            coro.setPrompt("exit");
        }
        auto val = coro.next();
        std::cout << val << ", ";
    }
    std::cout << std::endl;

    //ac::llama::Model model("D:/mod/Mistral-7B-Instruct-v0.1-GGUF/mistral-7b-instruct-v0.1.Q8_0.gguf");
    //ac::llama::Model model("D:/mod/gpt2/gguf/gpt2.Q6_K.gguf");
    ////ac::llama::Model model("D:/mod/gpt2/gguf-gpt2-chatbot/gpt2-chatbot.Q8_0.gguf");

    //ac::llama::Job job(model);

    //job.warmup();

    //job.setup("The rain in Turkey", {});
    ////job.decode("How to get to the moon?");

    //for (auto s : job.generate(100)) {
    //    std::cout << model.vocab().tokenToString(s);
    //}

    //std::cout << std::endl;

    //job.decode("Can I join NASA?");

    //for (auto s : job.generate()) {
    //    std::cout << model.vocab().tokenToString(s);
    //}

    //std::cout << std::endl;

    return 0;
}
