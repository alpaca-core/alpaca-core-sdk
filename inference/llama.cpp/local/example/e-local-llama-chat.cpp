// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/LocalLlama.hpp>

#include <ac/local/ModelFactory.hpp>
#include <ac/local/Model.hpp>
#include <ac/local/Instance.hpp>

#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>

#include <iostream>

#include "ac-test-data-llama-dir.h"

int main() try {
    ac::jalog::Instance jl;
    jl.setup().add<ac::jalog::sinks::DefaultSink>();

    ac::local::ModelFactory factory;
    ac::local::addLlamaInference(factory);

    auto model = factory.createModel(
        {
            .inferenceType = "llama.cpp",
            .assets = {
                {.path = AC_TEST_DATA_LLAMA_DIR "/gpt2-117m-q6_k.gguf"}
                //{.path = "D:/mod/Mistral-7B-Instruct-v0.1-GGUF/mistral-7b-instruct-v0.1.Q8_0.gguf"}
            }
        },
        {},
        [](std::string_view tag, float) {
            if (tag.empty()) {
                std::cout.put('*');
            }
            else {
                std::cout.put(tag[0]);
            }
            return true;
        }
    );


    auto instance = model->createInstance("general", {});

    std::string setup = "A chat between a human user and a helpful AI assistant.";

    std::cout << "Setup: " << setup << "\n";

    instance->runOp("begin-chat", {{"setup", std::move(setup)}});

    while (true) {
        std::cout << "User: ";
        std::string user;
        std::getline(std::cin, user);
        if (user == "/quit") break;
        user = ' ' + user;
        instance->runOp("add-chat-prompt", {{"prompt", user}});

        auto result = instance->runOp("get-chat-response", {});
        std::cout << "AI: " << result.at("response").get<std::string_view>() << '\n';
    }

    return 0;
}
catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
    return 1;
}
