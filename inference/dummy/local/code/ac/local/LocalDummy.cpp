// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalDummy.hpp"

#include <ac/dummy/Instance.hpp>
#include <ac/dummy/Model.hpp>

#include <ac/local/Instance.hpp>
#include <ac/local/Model.hpp>
#include <ac/local/ModelLoader.hpp>
#include <ac/local/ModelFactory.hpp>

#include <astl/move.hpp>
#include <astl/iile.h>
#include <astl/throw_ex.hpp>

namespace ac::local {

namespace {
dummy::Model::Params ModelParams_fromDict(const Dict& d) {
    dummy::Model::Params ret;
    Dict_optApplyValueAt(d, "splice", ret.splice);
    return ret;
}

dummy::Instance::InitParams InitParams_fromDict(const Dict& d) {
    dummy::Instance::InitParams ret;
    Dict_optApplyValueAt(d, "cutoff", ret.cutoff);
    return ret;
}

dummy::Instance::SessionParams SessionParams_fromDict(const Dict& d) {
    dummy::Instance::SessionParams ret;
    Dict_optApplyValueAt(d, "splice", ret.splice);
    Dict_optApplyValueAt(d, "throw_on", ret.throwOn);

    return ret;
}

class DummyInstance final : public Instance {
    std::shared_ptr<dummy::Model> m_model;
    dummy::Instance m_instance;
public:
    DummyInstance(std::shared_ptr<dummy::Model> model, dummy::Instance::InitParams params)
        : m_model(astl::move(model))
        , m_instance(*m_model, params)
    {}

    Dict run(Dict params) {
        auto input = params.at("input").get<std::vector<std::string>>();
        auto sparams = SessionParams_fromDict(params);

        auto s = m_instance.newSession(astl::move(input), sparams);

        std::string result;
        for (auto& w : s) {
            result += w;
            result += ' ';
        }
        if (!result.empty()) {
            // remove last space
            result.pop_back();
        }
        return {{"result", astl::move(result)}};
    }

    virtual Dict runOp(std::string_view op, Dict params, ProgressCb) override {
        if (op == "run") {
            return run(astl::move(params));
        }
        else {
            throw_ex{} << "dummy: unknown op: " << op;
        }
    }

    virtual bool haveStream() const noexcept override {
        return false;
    }
    virtual void pushStream(Dict) override {
        throw_ex{} << "dummy: pushStream not supported";
    }
    virtual std::optional<Dict> pullStream() override {
        throw_ex{} << "dummy: pushStream not supported";
    }
};

class DummyModel final : public Model {
    std::shared_ptr<dummy::Model> m_model;
public:
    DummyModel(const std::string& fname, dummy::Model::Params params)
        : m_model(std::make_shared<dummy::Model>(fname.c_str(), astl::move(params)))
    {}
    explicit DummyModel(dummy::Model::Params params) : m_model(std::make_shared<dummy::Model>(params)) {}

    virtual std::unique_ptr<Instance> createInstance(std::string_view type, Dict params) override {
        if (type != "general") {
            throw_ex{} << "dummy: unknown instance type: " << type;
        }
        return std::make_unique<DummyInstance>(m_model, InitParams_fromDict(params));
    }
};

class DummyModelLoader final : public ModelLoader {
public:
    virtual ModelPtr loadModel(ModelDesc desc, Dict params, ProgressCb pcb) override {
        if (desc.assets.size() > 1) throw_ex{} << "dummy: expected one or zero assets";

        auto modelParams = ModelParams_fromDict(params);

        if (desc.assets.empty()) {
            // synthetic model
            return std::make_shared<DummyModel>(std::move(modelParams));
        }
        else {
            auto& fname = desc.assets.front().path;
            if (pcb) {
                if (!pcb(fname, 0.1f)) {
                    throw_ex{} << "dummy: loading model aborted";
                }
            }
            return std::make_shared<DummyModel>(fname, std::move(modelParams));
        }
    }
};

} // namespace

void addDummyInference(ModelFactory& factory) {
    static DummyModelLoader loader;
    factory.addLoader("dummy", loader);
}

} // namespace ac::local
