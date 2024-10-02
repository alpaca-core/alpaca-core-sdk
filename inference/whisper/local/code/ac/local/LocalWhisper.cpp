// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalWhisper.hpp"

#include <ac/whisper/Instance.hpp>
#include <ac/whisper/Init.hpp>
#include <ac/whisper/Model.hpp>

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

class WhisperInstance final : public Instance {
    std::shared_ptr<whisper::Model> m_model;
    whisper::Instance m_instance;
public:
    WhisperInstance(std::shared_ptr<whisper::Model> model)
        : m_model(astl::move(model))
        , m_instance(*m_model, {})
    {}

    Dict run(Dict params) {
        auto value = params.find("audioBinaryMono");
        if (!value->is_binary()) {
            throw_ex{} << "missing input 'audioBinaryMono' in params";
        }

        auto pcmu8 = value->get_binary();
        auto pcmf32 = reinterpret_cast<float*>(pcmu8.data());
        auto pcmf32Size = pcmu8.size() / sizeof(float);

        auto result = m_instance.transcribe(std::span{pcmf32, pcmf32Size});
        return {{"result", astl::move(result)}};
    }

    virtual Dict runOp(std::string_view op, Dict params, ProgressCb) override {
        if (op == "transcribe") {
            return run(astl::move(params));
        }
        else {
            throw_ex{} << "whisper: unknown op: " << op;
        }
    }
};

class WhisperModel final : public Model {
    std::shared_ptr<whisper::Model> m_model;
public:
    WhisperModel(const std::string& gguf, whisper::Model::Params params)
        : m_model(std::make_shared<whisper::Model>(gguf.c_str(), astl::move(params)))
    {}

    virtual std::unique_ptr<Instance> createInstance(std::string_view type, Dict) override {
        if (type != "general") {
            throw_ex{} << "whisper: unknown instance type: " << type;
        }
        return std::make_unique<WhisperInstance>(m_model);
    }
};

class WhisperModelLoader final : public ModelLoader {
public:
    virtual ModelPtr loadModel(ModelDesc desc, Dict /*params*/, ProgressCb /*progressCb*/) override {
        if (desc.assets.size() != 1) throw_ex{} << "whisper: expected exactly one local asset";
        auto& bin = desc.assets.front().path;
        whisper::Model::Params modelParams;
        return std::make_shared<WhisperModel>(bin, modelParams);
    }
};
}

void addWhisperInference(ModelFactory& provider) {
    ac::whisper::initLibrary();

    static WhisperModelLoader loader;
    provider.addLoader("whisper.cpp", loader);
}

} // namespace ac
