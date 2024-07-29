// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalLlama.hpp"

#include <ac/llama/Instance.hpp>
#include <ac/llama/Init.hpp>
#include <ac/llama/Model.hpp>

#include <ac/LocalProvider.hpp>
#include <ac/LocalInference.hpp>
#include <astl/move.hpp>

namespace ac {

namespace {
class LlamaInstance final : public LocalInferenceInstance {
    llama::Instance m_instance;
public:
    LlamaInstance(llama::Model& model)
        : m_instance(model, {})
    {}

    void run(Dict params, std::function<void(Dict)> streamCb) {
        auto s = m_instance.newSession("The rain in Turkey", {});

        auto& model = m_instance.model();

        std::string result;
        for (int i = 0; i < 8; ++i) {
            auto t = s.getToken();
            if (t == ac::llama::Token_Invalid) {
                break;
            }
            result += model.vocab().tokenToString(t);
        }

        streamCb({{"result", astl::move(result)}});
    }

    virtual void runOp(std::string_view op, Dict params, std::function<void(Dict)> streamCb) override {
        if (op == "run") {
            run(astl::move(params), astl::move(streamCb));
        }
        else {
            throw std::runtime_error("unknown op");
        }
    }
};

class LlamaModel final : public LocalInferenceModel {
    llama::Model m_model;
public:
    LlamaModel(const std::string& gguf)
        : m_model(gguf.c_str(), {})
    {}

    virtual std::unique_ptr<LocalInferenceInstance> createInstance(std::string_view type, Dict params) override {
        return std::make_unique<LlamaInstance>(m_model);
    }
};

class LlamaModelLoader final : public LocalInferenceModelLoader {
public:
    virtual std::unique_ptr<LocalInferenceModel> loadModel(Dict params, std::function<void(float)> progress) override {
        auto gguf = params.at("gguf").get<std::string>();
        return std::make_unique<LlamaModel>(gguf);
    }
};
}

void addLocalLlamaInference(LocalProvider& provider) {
    ac::llama::initLibrary();

    static LlamaModelLoader loader;
    provider.addLocalInferenceLoader("llama.cpp", loader);
}

} // namespace ac
