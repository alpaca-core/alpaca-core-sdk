// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/Lib.hpp>
#include <ac/local/IoCtx.hpp>
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
    ac::frameio::BlockingIoCtx blockingCtx;
    ac::local::IoCtx io;

    auto& dummyProvider = ac::local::Lib::getProvider("dummy");
    ac::schema::BlockingIoHelper dummy(io.connect(dummyProvider), blockingCtx);

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
