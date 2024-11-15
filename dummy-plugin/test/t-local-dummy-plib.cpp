// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <aclp-dummy-plib.hpp>
#include <ac/local/ModelLoaderRegistry.hpp>

struct DummyRegistry : public ac::local::ModelLoaderRegistry {
    DummyRegistry() {
        add_dummy_to_ac_local(*this);
    }
};

#include "t-local-dummy.inl"
