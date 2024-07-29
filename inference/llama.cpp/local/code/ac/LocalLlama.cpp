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
#include <astl/iile.h>
#include <astl/throw_ex.hpp>

namespace ac {

namespace {
llama::Instance::SessionParams SessionParams_fromDict(const Dict& d) {
    llama::Instance::SessionParams ret;
    auto f = d.find("conversation");
    if (f != d.end()) {
        ret.conversation = f->get<bool>();
    }

    f = d.find("num_tokens");
    if (f != d.end()) {
        ret.numTokensToPredict = f->get<int>();
    }

    return ret;
}

class LlamaInstance final : public LocalInferenceInstance {
    llama::Instance m_instance;
public:
    LlamaInstance(llama::Model& model)
        : m_instance(model, {})
    {}

    void run(Dict params, std::function<void(Dict)> streamCb) {
        auto prompt = iile([&]() -> std::string {
            auto f = params.find("prompt");
            if (f == params.end()) return {};
            return astl::move(f->get<std::string>());
        });

        auto s = m_instance.newSession(astl::move(prompt), SessionParams_fromDict(params));

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
            throw_ex{} << "llama: unknown op: " << op;
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
        if (type != "general") {
            throw_ex{} << "llama: unknown instance type: " << type;
        }
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
