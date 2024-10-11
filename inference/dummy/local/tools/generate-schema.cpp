// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/DummyModelSchema.hpp>
#include <ac/local/ModelSchemaGenHelper.hpp>
#include <iostream>

int main(int argc, char** argv) {
    return ac::local::schema::generatorMain<ac::local::schema::Dummy>(argc, argv);
}
