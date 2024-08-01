// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

// a dummy synchronous provider used in tests only

#include <ac/Provider.hpp>
#include <ac/Model.hpp>
#include <ac/Instance.hpp>
#include <ac/api.h>

namespace {
class DummyInstance final : public ac::Instance {
public:
    void runOp(std::string_view op, ac::Dict params, ac::Callback<void, ac::Dict> cb) override {

    }

    void synchronize() override {

    }

    void initiateAbort(ac::Callback<void> cb) {

    }
};

class DummyModel final : public ac::Model {
public:
    void createInstance(std::string_view type, ac::Dict params, ac::Callback<ac::InstancePtr> cb) override {

    }
};

class DummyProvider final : public ac::Provider {
public:
    void createModel(ac::Dict params, ac::Callback<ac::ModelPtr> cb) override {

    }
};

} // anonymous namespace

extern "C" ac_api_provider* create_dummy_provider() {
    return reinterpret_cast<ac_api_provider*>(new DummyProvider);
}
