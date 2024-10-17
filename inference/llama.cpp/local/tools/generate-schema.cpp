// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/LlamaModelSchema.hpp>
#include <ac/schema/ModelSchemaGenHelper.hpp>

int main(int argc, char** argv) {
    using namespace ac::local::schema;
    return generatorMain<Llama>(argc, argv);
}
