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

    const std::string prompt = "The first person to";

    std::vector<std::string> antiprompts;
    antiprompts.push_back("user:"); // change it to "name" to break the token generation with the default input

    std::cout << "Prompt: " << prompt << "\n";
    for (size_t i = 0; i < antiprompts.size(); i++) {
        std::cout << "Antiprompt "<<"[" << i << "]" <<": \"" << antiprompts[i] << "\"\n";
    }
    std::cout << "Generation: " << "<prompt>" << prompt << "</prompt> ";

    auto result = instance->runOp("run", {{"prompt", prompt}, {"max_tokens", 20}, {"antiprompts", antiprompts}}, {});

    std::cout << result.at("result").get<std::string_view>() << '\n';

    return 0;
}
catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
    return 1;
}
