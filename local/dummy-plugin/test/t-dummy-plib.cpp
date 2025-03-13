// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <aclp-dummy-plib.hpp>

#include <ac/jalog/Fixture.inl>

struct GlobalFixture {
    GlobalFixture() {
        add_dummy_to_ac_local_plugin_manager();
    }
};
GlobalFixture globalFixture;

#include "t-dummy.inl"
