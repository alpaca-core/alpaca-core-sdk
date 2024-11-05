// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/LocalDummy.hpp>

#include <ac/local/ModelFactory.hpp>
#include <ac/local/Model.hpp>
#include <ac/local/Instance.hpp>

#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>

#include <iostream>

#include "ac-test-data-dummy-models.h"

int main() try {
    ac::jalog::Instance jl;
    jl.setup().add<ac::jalog::sinks::DefaultSink>();

    ac::local::ModelFactory factory;
    ac::local::addDummyInference(factory);

    auto model = factory.createModel({
        .inferenceType = "dummy",
        .assets = {
            {.path = AC_DUMMY_MODEL_LARGE, .tag = "x"}
        }
    }, {});

    auto instance = model->createInstance("general", {});

    auto opResult =  instance->runOp("run", {{"input", {"JFK", "said:"}}, {"splice", false}});

    std::cout << opResult << "\n";

    return 0;
}
catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
    return 1;
}
