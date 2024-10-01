// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/LocalLlama.hpp>

#include <ac/local/ModelFactory.hpp>
#include <ac/local/Model.hpp>
#include <ac/local/Instance.hpp>

#include <jalog/Instance.hpp>
#include <jalog/sinks/DefaultSink.hpp>

#include <iostream>

#include "ac-test-data-llama-dir.h"

int main() try {
    jalog::Instance jl;
    jl.setup().add<jalog::sinks::DefaultSink>();

    ac::local::ModelFactory factory;
    ac::local::addLlamaInference(factory);

    auto model = factory.createModel(
        {
            .inferenceType = "llama.cpp",
            .assets = {
                {.path = AC_TEST_DATA_LLAMA_DIR "/gpt2-117m-q6_k.gguf"}
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

    std::cout << "User: ";
    std::string user;
    std::getline(std::cin, user);
    setup += "User: " + user + "\nAssistant:";

    instance->runOp("chat", {{"setup", std::move(setup)}}, {});

    while (true) {
        if (user == " /quit") break;
        auto response = instance->pullStream();
        std::cout << "AI:" << response->at("response").get<std::string_view>();
        std::cout.put(' ');
        std::getline(std::cin, user);
        user = ' ' + user;
        instance->pushStream({{"prompt", user}});
    }

    return 0;
}
catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
    return 1;
}
