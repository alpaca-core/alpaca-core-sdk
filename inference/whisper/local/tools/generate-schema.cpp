// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/WhisperModelSchema.hpp>
#include <ac/local/ModelSchemaGenHelper.hpp>

int main(int argc, char** argv) {
    using namespace ac::local::schema;
    return generatorMain<Whisper>(argc, argv);
}
