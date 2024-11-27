// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <aclp-dummy-info.h>
#include <ac/local/Lib.hpp>
#include <ac/local/PluginManager.hpp>
#include <ac/local/ModelLoaderRegistry.hpp>
#include <doctest/doctest.h>

#include <ac-test-util/JalogFixture.inl>

struct GlobalFixture {
    GlobalFixture() {
        ac::local::Lib::pluginManager().loadPlugin(ACLP_dummy_PLUGIN_FILE);
    }
};
GlobalFixture globalFixture;

struct DummyRegistry : public ac::local::ModelLoaderRegistry {
    DummyRegistry() {
        auto& loaders = ac::local::Lib::modelLoaderRegistry().loaders();
        for (auto& loader : loaders) {
            addLoader(*loader.loader);
        }
    }
};

#include "t-dummy.inl"
