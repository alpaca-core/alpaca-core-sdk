// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

// a dummy synchronous provider used in tests only

#include <ac/Provider.hpp>
#include <ac/Model.hpp>
#include <ac/Instance.hpp>
#include <ac/ApiCUtil.hpp>

namespace {
class DummyInstance final : public ac::Instance {
    void runOp(std::string_view op, ac::Dict params, ac::Callback<void, ac::Dict> cb) override {
        if (op == "insta") {
            cb.progressCb({{"insta", "success"}});
            cb.resultCb({});
            return;
        }

        cb.progressCb({{"some", 42}});

        if (op == "error") {
            cb.resultCb(itlib::unexpected(ac::Error{params.at("error").get<std::string>()}));
        }
        else {
            cb.progressCb({{"more", 1024}});
            cb.resultCb({});
        }
    }

    void synchronize() override {} // not really tesable

    void initiateAbort(ac::Callback<void> cb) {
        cb.resultCb({});
    }
};

class DummyModel final : public ac::Model {
    void createInstance(std::string_view type, ac::Dict params, ac::Callback<ac::InstancePtr> cb) override {
        cb.progressCb(0.1f);
        if (type == "error") {
            cb.resultCb(itlib::unexpected(ac::Error{params.at("error").get<std::string>()}));
            return;
        }

        cb.progressCb(1.f);
        cb.resultCb(ac::InstancePtr{std::make_shared<DummyInstance>()});
    }
};

class DummyProvider final : public ac::Provider {
    void createModel(ac::Dict params, ac::Callback<ac::ModelPtr> cb) override {
        cb.progressCb(0.2f);
        cb.progressCb(0.5f);

        if (ac::Dict_optValueAt(params, "error", false)) {
            cb.resultCb(itlib::unexpected(ac::Error{"dummy error"}));
            return;
        }

        cb.progressCb(1.f);
        cb.resultCb(ac::ModelPtr{std::make_shared<DummyModel>()});
    }
};

} // anonymous namespace

extern "C" ac_api_provider* create_dummy_provider() {
    return ac::cutil::Provider_to_provider(new DummyProvider);
}
