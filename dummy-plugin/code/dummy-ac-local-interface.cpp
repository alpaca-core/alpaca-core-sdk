// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "dummy-ac-local-interface.hpp"
#include "ac/dummy/LocalDummy.hpp"

namespace ac::dummy {

void addToAcLocal(ac::local::ModelFactory& factory) {
    ac::dummy::addToModelFactory(factory);
}

}
