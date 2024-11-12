// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "add-dummy-to-ac-local.h"
#include "ac/dummy/LocalDummy.hpp"

extern "C"
void add_dummy_to_ac_local(ac::local::ModelFactory& factory) {
    ac::dummy::addToModelFactory(factory);
}
