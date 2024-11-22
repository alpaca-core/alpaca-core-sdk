// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <aclp-dummy-plib.hpp>
#include <ac/local/ModelLoaderRegistry.hpp>

#include <ac-test-util/JalogFixture.inl>

struct DummyRegistry : public ac::local::ModelLoaderRegistry {
    DummyRegistry() {
        add_dummy_to_ac_local_registry(*this);
    }
};

#include "t-dummy.inl"
