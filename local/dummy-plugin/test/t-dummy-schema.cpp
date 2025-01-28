// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/Lib.hpp>
#include <ac/frameio/local/LocalIoRunner.hpp>
#include <ac/schema/BlockingIoHelper.hpp>
#include <ac/schema/FrameHelpers.hpp>

#include <ac/local/PluginPlibUtil.inl>

#include <ac/dummy/LocalDummy.hpp>
#include <ac/dummy/DummyInterface.hpp>

#include <ac/jalog/Fixture.inl>

#include <doctest/doctest.h>

struct LoadDummyFixture {
    PlibHelper helper;
    LoadDummyFixture()
        : helper(ac::dummy::getPluginInterface())
    {
        helper.addProvidersToGlobalRegistry();
    }
};

LoadDummyFixture loadDummyFixture;

TEST_CASE("blocking io") {
    ac::frameio::LocalIoRunner io;

    auto dummyHandler = ac::local::Lib::createSessionHandler("dummy");
    ac::schema::BlockingIoHelper dummy(io.connectBlocking(std::move(dummyHandler)));

    namespace schema = ac::schema::dummy;

    dummy.expectState<schema::StateInitial>();
    dummy.call<schema::StateInitial::OpLoadModel>({});

    dummy.expectState<schema::StateModelLoaded>();

    CHECK_THROWS_WITH(
        dummy.call<schema::StateModelLoaded::OpCreateInstance>({.cutoff = 1000}),
        "error: Cutoff 1000 greater than model size 22"
    );

    dummy.call<schema::StateModelLoaded::OpCreateInstance>({.cutoff = 2});

    dummy.expectState<schema::StateInstance>();
    auto result = dummy.call<schema::StateInstance::OpRun>({
        .input = std::vector<std::string>{"a", "b", "c"}
    });
    CHECK(result.result == "a one b two c one");

    dummy.close();
}
