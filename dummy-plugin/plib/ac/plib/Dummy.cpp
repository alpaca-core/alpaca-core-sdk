// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Dummy.hpp"
#include <ac/dummy/LocalDummy.hpp>

namespace ac::local {
void addDummyToModelFactory(local::ModelFactory& factory) {
    dummy::addToModelFactory(factory);
}
} // namespace ac::local
