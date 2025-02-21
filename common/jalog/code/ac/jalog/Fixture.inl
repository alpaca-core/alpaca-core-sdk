// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

// fixture which globally initializes jalog with a DefaultSink
// suitable for tests and examples

// inline file, no include guard

#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>

struct JalogFixture {
    ac::jalog::Instance jl;
    JalogFixture() {
        jl.setup().async()
            .add<ac::jalog::sinks::DefaultSink>();
    }
};

JalogFixture jalogFixture;
