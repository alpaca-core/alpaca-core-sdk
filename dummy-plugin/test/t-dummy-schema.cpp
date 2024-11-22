// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <aclp-dummy-plib.hpp>
#include <ac/local/ModelLoaderRegistry.hpp>

#include <ac-test-util/JalogFixture.inl>

#include <doctest/doctest.h>

#include <dummy-schema.hpp>

struct LoadDummyFixture {
    LoadDummyFixture() {
        add_dummy_to_ac_local_global_registry();
    }
};

LoadDummyFixture loadDummyFixture;

TEST_CASE("dummy schema") {

}
