// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <iostream>

#include <ac/LocalProvider.hpp>
#include <ac/LocalLlama.hpp>
#include <ac/Model.hpp>
#include <ac/Instance.hpp>

#include <ac/llama/Init.hpp>
#include <ac/llama/Model.hpp>
#include <ac/llama/ChatFormat.hpp>
#include <ac/llama/Instance.hpp>

#include <jalog/Instance.hpp>
#include <jalog/sinks/ColorSink.hpp>

#include <vector>
#include <thread>

int main() try {
    jalog::Instance jl;
    jl.setup().add<jalog::sinks::ColorSink>();

    //auto d = ac::Dict::parse(R"({"a": {"x": 1, "y": 2}, "b": "asdf"})");
    //auto sub = std::move(d.at("a"));
    //std::cout << d.dump(2) << "\n";
    //std::cout << sub.dump(2) << "\n";
    //auto sub2 = std::move(d.at("b"));
    //std::cout << d.dump(2) << "\n";
    //std::cout << sub2.dump(2) << "\n";


    //ac::llama::initLibrary();

    ////ac::llama::Model model("D:/mod/Mistral-7B-Instruct-v0.1-GGUF/mistral-7b-instruct-v0.1.Q8_0.gguf", {});
    //ac::llama::Model model("D:/mod/gpt2/gguf/gpt2.Q6_K.gguf", {});
    ////ac::llama::Model model("D:/mod/gpt2/gguf-gpt2-chatbot/gpt2-chatbot.Q8_0.gguf", {});

    //ac::llama::Instance inst(model, {});

    //inst.warmup();

    //auto s = inst.newSession("The rain in Turkey", {});

    //for (int i= 0; i < 8; ++i) {
    //    auto t = s.getToken();
    //    if (t == ac::llama::Token_Invalid) {
    //        break;
    //    }
    //    std::cout << model.vocab().tokenToString(t);
    //}

    //std::cout << std::endl;

    //s.pushPrompt(" thousand in Ankara");

    //for (int i = 0; i < 20; ++i) {
    //    auto t = s.getToken();
    //    if (t == ac::llama::Token_Invalid) {
    //        break;
    //    }
    //    std::cout << model.vocab().tokenToString(t);
    //}

    //std::cout << std::endl;

    ac::LocalProvider lp;
    ac::addLocalLlamaInference(lp);

    lp.createModel({{"type", "llama.cpp"}, {"gguf", "D:/mod/gpt2/gguf/gpt2.Q6_K.gguf"}}, {
        [&](ac::CallbackResult<ac::ModelPtr> result) {
            if (result.has_error()) {
                std::cout << "model load error: " << result.error().text << "\n";
                return;
            }

            auto model = result.value();
            model->createInstance("xxx", {}, {
                [](ac::CallbackResult<ac::InstancePtr> result) {
                    if (result.has_error()) {
                        std::cout << "instance create error: " << result.error().text << "\n";
                        return;
                    }

                    auto inst = result.value();

                    inst->runOp("run", {}, {
                        [](ac::CallbackResult<void> result) {
                            if (result.has_error()) {
                                std::cout << "run error: " << result.error().text << "\n";
                                return;
                            }

                            std::cout << "run success\n";
                        },
                        [](ac::Dict result) {
                            std::cout << "run progress: " << result.dump(2) << "\n";
                        }
                    });
                }
            });
        },
        [](float f) {
            std::cout << "model load progress: " << f << "\n";
        }
    });

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    return 0;
}
catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
    return 1;
}