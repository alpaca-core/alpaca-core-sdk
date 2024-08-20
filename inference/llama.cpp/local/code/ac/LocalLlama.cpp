// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalLlama.hpp"

#include <ac/llama/Instance.hpp>
#include <ac/llama/Init.hpp>
#include <ac/llama/Model.hpp>
#include <ac/llama/AntipromptManager.hpp>

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

    return ret;
}

class LlamaInstance final : public LocalInferenceInstance {
    llama::Instance m_instance;
public:
    LlamaInstance(llama::Model& model)
        : m_instance(model, {})
    {}

    void run(Dict params, std::function<void(Dict)> streamCb) {
        auto prompt = Dict_optValueAt(params, "prompt", std::string{});
        auto antiprompts = Dict_optValueAt(params, "antiprompts", std::vector<std::string>{});
        const uint32_t maxTokens = Dict_optValueAt(params, "max_tokens", 2000u); // somewhat arbitrary, see #37

        auto s = m_instance.newSession(astl::move(prompt), SessionParams_fromDict(params));

        auto& model = m_instance.model();
        auto antiprompt = ac::llama::AntipromptManager(antiprompts);

        std::string result;
        for (uint32_t i = 0; i < maxTokens; ++i) {
            auto t = s.getToken();
            if (t == ac::llama::Token_Invalid) {
                break;
            }

            auto tokenStr = model.vocab().tokenToString(t);
            if (antiprompt.feedGeneratedText(tokenStr)) {
                break;
            }

            result += model.vocab().tokenToString(t);
        }

        streamCb({{"result", astl::move(result)}});
    }

    virtual void runOpSync(std::string_view op, Dict params, std::function<void(Dict)> streamCb) override {
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
    LlamaModel(const std::string& gguf, llama::ModelLoadProgressCb pcb, llama::Model::Params params)
        : m_model(gguf.c_str(), std::move(pcb), params)
    {}

    virtual std::unique_ptr<LocalInferenceInstance> createInstanceSync(std::string_view type, Dict) override {
        if (type != "general") {
            throw_ex{} << "llama: unknown instance type: " << type;
        }
        return std::make_unique<LlamaInstance>(m_model);
    }
};

class LlamaModelLoader final : public LocalInferenceModelLoader {
public:
    virtual std::unique_ptr<LocalInferenceModel> loadModelSync(Dict params, std::function<void(float)> progressCb) override {
        auto gguf = params.at("gguf").get<std::string>();
        llama::Model::Params modelParams;
        return std::make_unique<LlamaModel>(gguf, std::move(progressCb), modelParams);
    }
};
}

void addLocalLlamaInference(LocalProvider& provider) {
    ac::llama::initLibrary();

    static LlamaModelLoader loader;
    provider.addLocalInferenceLoader("llama.cpp", loader);
}

} // namespace ac
