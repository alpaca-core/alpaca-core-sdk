// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <aclp-dummy-info.h>
#include <ac/local/Lib.hpp>
#include <ac/local/PluginManager.hpp>
#include <doctest/doctest.h>

#include <ac/jalog/Fixture.inl>

struct GlobalFixture {
    GlobalFixture() {
        ac::local::Lib::pluginManager().loadPlugin(ACLP_dummy_PLUGIN_FILE);
    }
};
GlobalFixture globalFixture;

#include "t-dummy.inl"
