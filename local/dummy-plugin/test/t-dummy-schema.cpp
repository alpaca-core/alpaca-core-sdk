// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/Lib.hpp>
#include <ac/local/Model.hpp>
#include <ac/local/Instance.hpp>
#include <ac/local/ProviderRegistry.hpp>

#include <ac/local/schema/CallHelpers.hpp>

#include <ac/local/PluginPlibUtil.inl>

#include <ac/dummy/LocalDummy.hpp>
#include <ac/dummy/DummyProviderSchema.hpp>

#include <ac/jalog/Fixture.inl>

#include <doctest/doctest.h>

#include <ac-test-data-dummy-models.h>

struct LoadDummyFixture {
    PlibHelper helper;
    LoadDummyFixture()
        : helper(ac::dummy::getPluginInterface())
    {
        helper.addProvidersToGlobalRegistry();
    }
};

LoadDummyFixture loadDummyFixture;

TEST_CASE("dummy schema") {
    auto model = ac::local::Lib::loadModel({
        .type = "dummy",
        .name = "synthetic"
        }, {});

    REQUIRE(!!model);

    using Instance = ac::schema::DummyProvider::InstanceGeneral;
    auto instance = Model_createInstance<Instance>(*model, {.cutoff = 2});

    using Interface = ac::schema::DummyInterface;
    auto result = Instance_runOp<Interface::OpRun>(*instance,
        {.input = std::vector<std::string>{"a", "b", "c"}}
    );
    CHECK(result.result == "a one b two c one");
}
