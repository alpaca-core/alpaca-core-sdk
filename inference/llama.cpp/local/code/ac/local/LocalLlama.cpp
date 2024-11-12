// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalLlama.hpp"
#include "LlamaModelSchema.hpp"

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
#include <astl/throw_stdex.hpp>
#include <astl/workarounds.h>

namespace ac::local {

namespace {
class ChatSession {
    llama::Session m_session;
    const llama::Vocab& m_vocab;
    std::string m_userPrefix;
    std::string m_assistantPrefix;

    std::vector<llama::Token> m_promptTokens;

    bool m_addUserPrefix = true;
    bool m_addAssistantPrefix = true;
public:
    using Schema = ac::local::schema::Llama::InstanceGeneral;

    ChatSession(llama::Instance& instance, Dict& params)
        : m_session(instance.newSession({}))
        , m_vocab(instance.model().vocab())
    {
        Schema::OpChatBegin::Params schemaParams(params);
        auto setup = schemaParams.setup.getValue();
        m_promptTokens = instance.model().vocab().tokenize(setup, true, true);
        m_session.setInitialPrompt(m_promptTokens);

        m_userPrefix = "\n";
        m_userPrefix += schemaParams.roleUser.getValue();
        m_userPrefix += ":";
        m_assistantPrefix = "\n";
        m_assistantPrefix += schemaParams.roleAssistant.getValue();;
        m_assistantPrefix += ":";
    }

    void pushPrompt(Dict& params) {
        Schema::OpChatAddPrompt::Params schemaParams(params);
        auto prompt = std::string(schemaParams.prompt.getValue());

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
        for (int i=0; i<1000; ++i) {
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

        Dict ret;
        Schema::OpChatGetResponse::Return result(ret);
        result.response.setValue(astl::move(response));
        return ret;
    }
};


class LlamaInstance final : public Instance {
    std::shared_ptr<llama::Model> m_model;
    llama::Instance m_instance;

    std::optional<ChatSession> m_chatSession;

public:
    using Schema = ac::local::schema::Llama::InstanceGeneral;

    LlamaInstance(std::shared_ptr<llama::Model> model, llama::Instance::InitParams params)
        : m_model(astl::move(model))
        , m_instance(*m_model, astl::move(params))
    {}

    Dict run(Dict& params) {
        Schema::OpRun::Params schemaParams(params);
        auto prompt = schemaParams.prompt.getValue();
        const auto maxTokens = schemaParams.maxTokens.getValue();

        auto s = m_instance.newSession({});

        auto promptTokens = m_instance.model().vocab().tokenize(prompt, true, true);
        s.setInitialPrompt(promptTokens);

        auto& model = m_instance.model();
        ac::llama::AntipromptManager antiprompt;

        auto& antiprompts = schemaParams.antiprompts;
        for (size_t i = 0; i < antiprompts.size(); ++i) {
            antiprompt.addAntiprompt(antiprompts[i].getValue());
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
        switch (Schema::getOpIndexById(op)) {
        case Schema::opIndex<Schema::OpRun>:
            return run(params);
        case Schema::opIndex<Schema::OpChatBegin>:
            m_chatSession.emplace(m_instance, params);
            return {};
        case Schema::opIndex<Schema::OpChatAddPrompt>:
            if (!m_chatSession) {
                throw_ex{} << "llama: chat not started";
            }
            m_chatSession->pushPrompt(params);
            return {};
        case Schema::opIndex<Schema::OpChatGetResponse>:
            if (!m_chatSession) {
                throw_ex{} << "llama: chat not started";
            }
            return m_chatSession->getResponse();
        default:
            throw_ex{} << "llama: unknown op: " << op;
            MSVC_WO_10766806();
        }
    }
};

class LlamaModel final : public Model {
    std::shared_ptr<llama::Model> m_model;

    llama::Instance::InitParams translateInstanceParams(Dict& params) {
        llama::Instance::InitParams initParams;
        Schema::InstanceGeneral::Params schemaParams(params);

        if (auto value = schemaParams.ctxSize.optGetValue()) {
            initParams.ctxSize = *value;
        }

        if (auto value = schemaParams.batchSize.optGetValue()) {
            initParams.batchSize = *value;
        }

        if (auto value = schemaParams.ubatchSize.optGetValue()) {
            initParams.ubatchSize = *value;
        }

        return initParams;
    }
public:
    using Schema = ac::local::schema::Llama;

    LlamaModel(const std::string& gguf, llama::ModelLoadProgressCb pcb, llama::Model::Params params)
        : m_model(std::make_shared<llama::Model>(gguf.c_str(), astl::move(pcb), astl::move(params)))
    {}

    virtual std::unique_ptr<Instance> createInstance(std::string_view type, Dict params) override {
        switch (Schema::getInstanceById(type)) {
        case Schema::instanceIndex<Schema::InstanceGeneral>:
            return std::make_unique<LlamaInstance>(m_model, translateInstanceParams(params));
        default:
            throw_ex{} << "llama: unknown instance type: " << type;
            MSVC_WO_10766806();
        }
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
