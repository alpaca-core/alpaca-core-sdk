// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

// inline file, no include guard

#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>

struct JalogFixture {
    ac::jalog::Instance jl;
    JalogFixture() {
        jl.setup()
            .add<ac::jalog::sinks::DefaultSink>();
    }
};

JalogFixture jalogFixture;
