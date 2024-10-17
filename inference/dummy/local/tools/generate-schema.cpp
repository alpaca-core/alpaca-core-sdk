// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/DummyModelSchema.hpp>
#include <ac/schema/ModelSchemaGenHelper.hpp>

int main(int argc, char** argv) {
    using namespace ac::local::schema;
    return generatorMain<Dummy>(argc, argv);
}
