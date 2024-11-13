// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <aclp-dummy-plib.hpp>
#include <ac/local/ModelFactory.hpp>

struct DummyFactory : public ac::local::ModelFactory {
    DummyFactory() {
        add_dummy_to_ac_local(*this);
    }
};

#include "t-local-dummy.inl"
