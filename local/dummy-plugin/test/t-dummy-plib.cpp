// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <aclp-dummy-plib.hpp>
#include <ac/local/ProviderRegistry.hpp>

#include <ac/jalog/Fixture.inl>

struct DummyRegistry : public ac::local::ProviderRegistry {
    DummyRegistry() {
        add_dummy_to_ac_local_registry(*this);
    }
};

#include "t-dummy.inl"
