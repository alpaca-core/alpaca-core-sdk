// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/Lib.hpp>
#include <ac/frameio/local/LocalIoRunner.hpp>
#include <ac/frameio/local/BlockingIo.hpp>
#include <ac/schema/FrameHelpers.hpp>

#include <ac/local/PluginPlibUtil.inl>

#include <ac/dummy/LocalDummy.hpp>
#include <ac/dummy/DummyProviderSchema.hpp>

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

TEST_CASE("dummy schema") {
    ac::frameio::LocalIoRunner io;
    auto dummyHandler = ac::local::Lib::createSessionHandler("dummy");
    auto dummy = io.connectBlocking(std::move(dummyHandler));

    using Run = ac::schema::DummyInterface::OpRun;

    CHECK(dummy.push({"load_model", {}}).success());
    CHECK(dummy.push({"create_instance", {{"cutoff", 2}}}).success());
    CHECK(dummy.push(Frame_fromOpParams(Run{}, {
        .input = std::vector<std::string>{"a", "b", "c"}
    })).success());

    auto result = Frame_toOpReturn(Run{}, dummy.poll().frame);
    CHECK(result.result == "a one b two c one");

    dummy.close();
}
