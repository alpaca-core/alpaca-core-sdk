// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalLlama.hpp"
#include "LocalLlamaSchema.hpp"

#include <ac/llama/Instance.hpp>
#include <ac/llama/Init.hpp>
#include <ac/llama/Model.hpp>
#include <ac/llama/AntipromptManager.hpp>

#include <ac/LocalProvider.hpp>
#include <ac/LocalModelInfo.hpp>
#include <ac/LocalInference.hpp>

#include <astl/move.hpp>
#include <astl/move_capture.hpp>
#include <astl/iile.h>
#include <astl/throw_ex.hpp>

namespace ac {

namespace {
llama::Instance::SessionParams SessionParams_fromDict(const Dict& d) {
    llama::Instance::SessionParams ret;
    Dict_optApplyValueAt(d, "conversation", ret.conversation);
    return ret;
}

class LlamaInstance final : public LocalInferenceInstance {
    llama::Instance m_instance;
public:
    LlamaInstance(llama::Model& model)
        : m_instance(model, {})
    {}

    void run(Dict params, BasicCb<Dict> streamCb) {
        auto prompt = ac::LlamaSchema::RunParams::prompt(params);
        auto antiprompts = ac::LlamaSchema::RunParams::antiprompts(params);
        const uint32_t maxTokens = ac::LlamaSchema::RunParams::max_tokens(params);

        auto s = m_instance.newSession(astl::move(prompt), SessionParams_fromDict(params));

        auto& model = m_instance.model();
        ac::llama::AntipromptManager antiprompt;
        for (const auto& ap : antiprompts) {
            antiprompt.addAntiprompt(ap);
        }

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

        Dict resultDict;
        ac::LlamaSchema::RunResult::set_result(resultDict, astl::move(result));
        streamCb(resultDict);
    }

    virtual void runOpSync(std::string_view op, Dict params, BasicCb<Dict> streamCb, ProgressCb) override {
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
    virtual std::unique_ptr<LocalInferenceModel> loadModelSync(LocalModelInfoPtr info, Dict, ProgressCb progressCb) override {
        if (!info) throw_ex{} << "llama: no model info";
        if (info->localAssets.size() != 1) throw_ex{} << "llama: expected exactly one local asset";
        auto& gguf = info->localAssets.front().path;
        if (!gguf) throw_ex{} << "llama: missing gguf path";
        llama::Model::Params modelParams;
        std::string progressTag = "loading " + *gguf;
        return std::make_unique<LlamaModel>(*gguf, [movecap(progressTag, progressCb)](float p) {
            if (progressCb) {
                progressCb(progressTag, p);
            }
        }, modelParams);
    }
};
}

void addLocalLlamaInference(LocalProvider& provider) {
    ac::llama::initLibrary();

    static LlamaModelLoader loader;
    provider.addLocalInferenceLoader("llama.cpp", loader);
}

} // namespace ac
