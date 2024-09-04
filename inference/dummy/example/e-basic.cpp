// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/dummy/Model.hpp>
#include <ac/dummy/Instance.hpp>

// logging
#include <jalog/Instance.hpp>
#include <jalog/sinks/ColorSink.hpp>

// models
#include "ac-test-data-dummy-models.h"

#include <iostream>

int main() try {
    jalog::Instance jl;
    jl.setup().add<jalog::sinks::ColorSink>();

    ac::dummy::Model model(AC_DUMMY_MODEL_SMALL, {});
    ac::dummy::Instance instance(model, {});

    auto s = instance.newSession({"soco", "bate", "soco", "vira"}, {.splice = false});

    for (const auto& token : s) {
        std::cout << token << ' ';
    }

    return 0;
}
catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
