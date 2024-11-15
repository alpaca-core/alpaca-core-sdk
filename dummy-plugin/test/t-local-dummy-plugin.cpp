// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <aclp-dummy-info.h>
#include <ac/local/PluginLoader.hpp>
#include <ac/local/ModelFactory.hpp>
#include <doctest/doctest.h>
#include <optional>

std::optional<ac::local::PluginInterface> dummyPluginInterface;

struct GlobalFixture {
    GlobalFixture() {
        dummyPluginInterface.emplace(ac::local::PluginLoader::loadPlugin(ACLP_dummy_PLUGIN_FILE));
    }
};
GlobalFixture globalFixture;

struct DummyFactory : public ac::local::ModelFactory {
    DummyFactory() {
        REQUIRE(dummyPluginInterface.has_value());
        dummyPluginInterface->addLoadersToFactory(*this);
    }
};

#include "t-local-dummy.inl"
