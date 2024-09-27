// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "local_llama.h"
#include "local.h"
#include <ac/local/LocalLlama.hpp>

extern "C" void ac_add_local_llama_inference(struct ac_local_model_factory* factory) {
    ac::local::addLlamaInference(ac::local::cutil::ModelFactory_toCpp(factory));
}
