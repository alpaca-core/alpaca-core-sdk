// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <aclp-dummy-info.h>
#include <ac/local/Lib.hpp>
#include <ac/local/PluginManager.hpp>
#include <ac/local/ProviderRegistry.hpp>
#include <doctest/doctest.h>

#include <ac-test-util/JalogFixture.inl>

struct GlobalFixture {
    GlobalFixture() {
        ac::local::Lib::pluginManager().loadPlugin(ACLP_dummy_PLUGIN_FILE);
    }
};
GlobalFixture globalFixture;

struct DummyRegistry : public ac::local::ProviderRegistry {
    DummyRegistry() {
        auto& providers = ac::local::Lib::providerRegistry().providers();
        for (auto& provider : providers) {
            addProvider(*provider.provider);
        }
    }
};

#include "t-dummy.inl"
