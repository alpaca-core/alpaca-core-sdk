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

class ChatSession {
    llama::Session m_session;
    const llama::Vocab& m_vocab;
    std::string m_userPrefix;
    std::string m_assistantPrefix;

    std::vector<llama::Token> m_promptTokens;

    bool m_addUserPrefix = true;
    bool m_addAssistantPrefix = true;
public:
    ChatSession(llama::Instance& instance, Dict& params)
        : m_session(instance.newSession(SessionParams_fromDict(params)))
        , m_vocab(instance.model().vocab())
    {
        auto setup = Dict_optValueAt(params, "setup", std::string_view{});
        m_promptTokens = instance.model().vocab().tokenize(setup, true, true);
        m_session.setInitialPrompt(m_promptTokens);

        m_userPrefix = "\n";
        m_userPrefix += Dict_optValueAt(params, "role_user", std::string_view("User"));
        m_userPrefix += ":";
        m_assistantPrefix = "\n";
        m_assistantPrefix += Dict_optValueAt(params, "role_assistant", std::string_view("Assistant"));
        m_assistantPrefix += ":";
    }

    void pushPrompt(Dict& params) {
        auto prompt = Dict_optValueAt(params, "prompt", std::string{});

        // prefix with space as the generated content doesn't include it
        prompt = ' ' + prompt;

        if (m_addUserPrefix) {
            // we haven't had an interaction yet, so we need to add the user prefix
            // subsequent interaction will have it generated
            prompt = m_userPrefix + prompt;
        }

        // prepare for the next generation
        prompt += m_assistantPrefix;

        m_promptTokens = m_vocab.tokenize(prompt, false, false);
        m_session.pushPrompt(m_promptTokens);
        m_addAssistantPrefix = false;
    }

    Dict getResponse() {
        if (m_addAssistantPrefix) {
            // genrated responses are requested first, but we haven't yet fed the assistant prefix to the model
            auto prompt = m_assistantPrefix;
            assert(m_promptTokens.empty()); // nothing should be pending here
            m_promptTokens = m_vocab.tokenize(prompt, false, false);
            m_session.pushPrompt(m_promptTokens);
        }


        ac::llama::IncrementalStringFinder finder(m_userPrefix);

        m_addUserPrefix = true;
        std::string response;
        while (true) {
            auto t = m_session.getToken();
            if (t == ac::llama::Token_Invalid) {
                // no more tokens
                break;
            }

            auto tokenStr = m_vocab.tokenToString(t);
            response += tokenStr;

            if (finder.feedText(tokenStr)) {
                // user prefix was added by generation, so don't add it again
                m_addUserPrefix = false;

                // and also hide it from the return value
                // note that we assume that m_userPrefix is always the final piece of text in the response
                // TODO: update to better match the cutoff when issue #131 is done
                response.resize(response.size() - m_userPrefix.size());
                break;
            }
        }

        // remove leading space if any
        // we could add the space to the assistant prefix, but most models have a much easier time generating tokens
        // with a leading space, so instead of burdening them with "unorthodox" tokens, we'll clear it here
        if (!response.empty() && response[0] == ' ') {
            response.erase(0, 1);
        }

        return {{"response", astl::move(response)}};
    }
};


class LlamaInstance final : public Instance {
    std::shared_ptr<llama::Model> m_model;
    llama::Instance m_instance;

    std::optional<ChatSession> m_chatSession;

public:
    LlamaInstance(std::shared_ptr<llama::Model> model)
        : m_model(astl::move(model))
        , m_instance(*m_model, {})
    {}

    Dict run(Dict& params) {
        auto prompt = Dict_optValueAt(params, "prompt", std::string_view{});
        auto antiprompts = Dict_optValueAt(params, "antiprompts", std::vector<std::string_view>{});
        const auto maxTokens = Dict_optValueAt(params, "max_tokens", uint32_t(0));

        auto s = m_instance.newSession(SessionParams_fromDict(params));

        auto promptTokens = m_instance.model().vocab().tokenize(prompt, true, true);
        s.setInitialPrompt(promptTokens);

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

    virtual Dict runOp(std::string_view op, Dict params, ProgressCb) override {
        if (op == "run") {
            if (m_chatSession) {
                m_chatSession.reset();
            }
            return run(params);
        }
        if (op == "begin-chat") {
            m_chatSession.emplace(m_instance, params);
            return {};
        }
        if (op == "add-chat-prompt") {
            if (!m_chatSession) {
                throw_ex{} << "llama: chat not started";
            }
            m_chatSession->pushPrompt(params);
            return {};
        }
        if (op == "get-chat-response") {
            if (!m_chatSession) {
                throw_ex{} << "llama: chat not started";
            }
            return m_chatSession->getResponse();
        }
        else {
            throw_ex{} << "llama: unknown op: " << op;
        }
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
