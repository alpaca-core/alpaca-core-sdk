// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalLlama.hpp"

#include <ac/llama/Session.hpp>
#include <ac/llama/Instance.hpp>
#include <ac/llama/Init.hpp>
#include <ac/llama/Model.hpp>
#include <ac/llama/AntipromptManager.hpp>

#include <ac/local/Instance.hpp>
#include <ac/local/Model.hpp>
#include <ac/local/ModelLoader.hpp>
#include <ac/local/ModelFactory.hpp>

#include <astl/move.hpp>
#include <astl/move_capture.hpp>
#include <astl/iile.h>
#include <astl/throw_ex.hpp>

namespace ac::local {

namespace {
llama::Instance::SessionParams SessionParams_fromDict(const Dict&) {
    llama::Instance::SessionParams ret;
    return ret;
}

class LlamaInstance final : public Instance {
    std::shared_ptr<llama::Model> m_model;
    llama::Instance m_instance;

    std::vector<llama::Token> m_promptTokens;

    struct ChatSession {
        llama::Session session;
        std::string userPrefix;
        std::string assistantPrefix;

        explicit operator bool() const noexcept {
            return !!session;
        }
    };

    ChatSession m_chatSession;

public:
    LlamaInstance(std::shared_ptr<llama::Model> model)
        : m_model(astl::move(model))
        , m_instance(*m_model, {})
    {}

    Dict run(Dict& params) {
        auto prompt = Dict_optValueAt(params, "prompt", std::string_view{});
        auto antiprompts = Dict_optValueAt(params, "antiprompts", std::vector<std::string_view>{});
        const auto maxTokens = Dict_optValueAt(params, "max_tokens", uint32_t(0));

        m_promptTokens = m_instance.model().vocab().tokenize(prompt, true, true);
        auto s = m_instance.newSession(SessionParams_fromDict(params));
        s.setInitialPrompt(m_promptTokens);

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

            result += tokenStr;
        }

        return {{"result", astl::move(result)}};
    }

    Dict chat(Dict& params) {
        auto setup = Dict_optValueAt(params, "setup", std::string_view{});
        auto roleUser = Dict_optValueAt(params, "role_user", std::string_view("User"));
        auto roleAssistant = Dict_optValueAt(params, "role_assistant", std::string_view("Assistant"));

        m_chatSession.session = m_instance.newSession(SessionParams_fromDict(params));
        m_chatSession.userPrefix = roleUser;
        m_chatSession.userPrefix += ":";
        m_chatSession.assistantPrefix = roleAssistant;
        m_chatSession.assistantPrefix += ":";

        m_promptTokens = m_model->vocab().tokenize(setup, true, true);
        m_chatSession.session.setInitialPrompt(m_promptTokens);

        return {};
    }

    virtual Dict runOp(std::string_view op, Dict params, ProgressCb) override {
        if (m_chatSession) {
            m_chatSession = {};
        }

        if (op == "run") {
            return run(params);
        }
        if (op == "chat") {
            return chat(params);
        }
        else {
            throw_ex{} << "llama: unknown op: " << op;
        }
    }

    virtual bool haveStream() const noexcept override {
        return !!m_chatSession;
    }
    virtual void pushStream(Dict params) override {
        if (!m_chatSession) {
            throw_ex{} << "llama: no stream available";
        }
        auto prompt = Dict_optValueAt(params, "prompt", std::string{});
        prompt = prompt + "\n" + m_chatSession.assistantPrefix;
        m_promptTokens = m_model->vocab().tokenize(prompt, true, true);
        m_chatSession.session.pushPrompt(m_promptTokens);
    }
    virtual std::optional<Dict> pullStream() override {
        if (!m_chatSession) {
            throw_ex{} << "llama: no stream available";
        }
        std::string result;
        ac::llama::IncrementalStringFinder finder(m_chatSession.userPrefix);
        while (true) {
            auto t = m_chatSession.session.getToken();
            if (t == ac::llama::Token_Invalid) {
                break;
            }

            auto tokenStr = m_model->vocab().tokenToString(t);
            result += tokenStr;

            if (finder.feedText(tokenStr)) {
                break;
            }
        }
        return ac::Dict{{"response", astl::move(result)}};
    }
};

class LlamaModel final : public Model {
    std::shared_ptr<llama::Model> m_model;
public:
    LlamaModel(const std::string& gguf, llama::ModelLoadProgressCb pcb, llama::Model::Params params)
        : m_model(std::make_shared<llama::Model>(gguf.c_str(), astl::move(pcb), astl::move(params)))
    {}

    virtual std::unique_ptr<Instance> createInstance(std::string_view type, Dict) override {
        if (type != "general") {
            throw_ex{} << "llama: unknown instance type: " << type;
        }
        return std::make_unique<LlamaInstance>(m_model);
    }
};

class LlamaModelLoader final : public ModelLoader {
public:
    virtual ModelPtr loadModel(ModelDesc desc, Dict, ProgressCb progressCb) override {
        if (desc.assets.size() != 1) throw_ex{} << "llama: expected exactly one local asset";
        auto& gguf = desc.assets.front().path;
        llama::Model::Params modelParams;
        std::string progressTag = "loading " + gguf;
        return std::make_shared<LlamaModel>(gguf, [movecap(progressTag, progressCb)](float p) {
            if (progressCb) {
                progressCb(progressTag, p);
            }
        }, astl::move(modelParams));
    }
};
}

void addLlamaInference(ModelFactory& provider) {
    ac::llama::initLibrary();

    static LlamaModelLoader loader;
    provider.addLoader("llama.cpp", loader);
}

} // namespace ac
