// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "local_dummy.h"
#include <ac/local.h>
#include <ac/local/LocalDummy.hpp>

extern "C" {
void ac_add_local_dummy_inference(struct ac_local_model_factory* factory) {
    ac::local::addDummyInference(ac::local::cutil::ModelFactory_toCpp(factory));
}
}
