// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/Lib.hpp>
#include <ac/local/DefaultBackend.hpp>
#include <ac/local/PluginManager.hpp>
#include <ac/schema/BlockingIoHelper.hpp>
#include <ac/schema/FrameHelpers.hpp>

#include <ac/dummy/LocalDummy.hpp>
#include <ac/dummy/DummyInterface.hpp>

#include <ac/jalog/Fixture.inl>

#include <doctest/doctest.h>

struct LoadDummyFixture {
    LoadDummyFixture() {
        ac::local::Lib::pluginManager().loadPlib(ac::dummy::getPluginInterface());
    }
};

LoadDummyFixture loadDummyFixture;

TEST_CASE("blocking io") {
    ac::local::DefaultBackend backend("dummy-test");

    ac::schema::BlockingIoHelper dummy(backend.connect("dummy", {}));

    namespace schema = ac::schema::dummy;

    auto sid = dummy.poll<ac::schema::StateChange>();
    CHECK(sid == schema::StateDummy::id);

    sid = dummy.call<schema::StateDummy::OpLoadModel>({});
    CHECK(sid == schema::StateModelLoaded::id);

    CHECK_THROWS_WITH(
        dummy.call<schema::StateModelLoaded::OpCreateInstance>({ .cutoff = 1000 }); ,
        "error: Cutoff 1000 greater than model size 22"
    );

    sid = dummy.call<schema::StateModelLoaded::OpCreateInstance>({.cutoff = 2});
    CHECK(sid == schema::StateInstance::id);

    auto result = dummy.call<schema::StateInstance::OpRun>({
        .input = std::vector<std::string>{"a", "b", "c"}
    });
    CHECK(result.result == "a one b two c one");

    auto stream = dummy.stream<schema::StateInstance::OpStream>({
        .input = std::vector<std::string>{"x", "y"}
    });
    CHECK(stream.next() == "x");
    CHECK(stream.next() == "one");
    CHECK(stream.next() == "y");
    CHECK(stream.next() == "two");
    CHECK_FALSE(stream.next());
    CHECK(stream.rval() == nullptr);

    dummy.close();
}
