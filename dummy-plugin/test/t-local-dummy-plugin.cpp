// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <aclp-dummy-info.h>
#include <ac/local/PluginLoader.hpp>
#include <ac/local/ModelLoaderRegistry.hpp>
#include <doctest/doctest.h>
#include <optional>

std::optional<ac::local::PluginInterface> dummyPluginInterface;

struct GlobalFixture {
    GlobalFixture() {
        dummyPluginInterface.emplace(ac::local::PluginLoader::loadPlugin(ACLP_dummy_PLUGIN_FILE));
    }
};
GlobalFixture globalFixture;

struct DummyRegistry : public ac::local::ModelLoaderRegistry {
    DummyRegistry() {
        addLoaders(dummyPluginInterface->getLoaders());
    }
};

#include "t-local-dummy.inl"
