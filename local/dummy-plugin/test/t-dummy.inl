// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

// inline file
// needs a definition of DummyRegistry to be complete

#include "ac-test-data-dummy-models.h"

#include <ac/local/Model.hpp>
#include <ac/local/Instance.hpp>

#include <ac/frameio/local/BlockingSyncIoWrapper.hpp>

#include <doctest/doctest.h>

#include <astl/move.hpp>

const ac::local::ModelAssetDesc Model_Desc = {
    .type = "dummy",
    .assets = {
        {.path = AC_DUMMY_MODEL_SMALL}
    }
};

using Session = ac::frameio::BlockingSyncIoWrapper;

Session createTestSession(DummyRegistry& d) {
    auto h = d.createSessionHandler("dummy");
    REQUIRE(h);
    return ac::frameio::BlockingSyncIoWrapper(std::move(h));
}

void checkError(Session& s, const std::string_view msg) {
    auto res = s.poll();
    CHECK(res.success());
    auto& frame = res.frame;
    CHECK(frame.op == "error");
    CHECK(frame.data.get<std::string_view>() == msg);
    CHECK(s.poll().closed());
}

void checkRunResult(Session& s, const std::string_view msg) {
    auto res = s.poll();
    CHECK(res.success());
    auto& frame = res.frame;
    CHECK(frame.op == "run");
    CHECK(frame.data.at("result").get<std::string>() == msg);
}

TEST_CASE("bad model") {
    DummyRegistry d;

    {
        auto s = createTestSession(d);
        CHECK(s.poll().blocked());
        s.push({ "nope", {} });
        checkError(s, "dummy: expected 'load' op, got: nope");
    }

    {
        auto s = createTestSession(d);
        s.push({"load", {{"file_path", "nope"}}});
        checkError(s, "Failed to open file: nope");
    }
}

TEST_CASE("bad instance") {
    DummyRegistry d;
    auto s = createTestSession(d);

    s.push({"load", {{"file_path", AC_DUMMY_MODEL_SMALL}}});
    CHECK(s.poll().blocked());

    SUBCASE("bad op") {
        s.push({ "nope", {} });
        checkError(s, "dummy: expected 'create' op, got: nope");
    }
    SUBCASE("bad params") {
        s.push({ "create", {{"cutoff", 40}} });
        checkError(s, "Cutoff 40 greater than model size 3");
    }
}

TEST_CASE("general") {
    DummyRegistry d;

    auto s = createTestSession(d);

    s.push({"load", {{"file_path", AC_DUMMY_MODEL_SMALL}}});
    CHECK(s.poll().blocked());

    s.push({"create", {}});
    CHECK(s.poll().blocked());

    SUBCASE("bad op") {
        s.push({"nope", {}});
        checkError(s, "dummy: unknown op: nope");
    }
    SUBCASE("bad args") {
        s.push({ "run", {{"foo", "nope"}} });
        checkError(s, "Required field input is not set");
    }
    SUBCASE("request error") {

        s.push({"run", {{"input", {"a", "b"}}}});
        checkRunResult(s, "a soco b bate");

        s.push({"run", {{"input", {"a", "b"}}, {"splice", false}}});
        checkRunResult(s, "a b soco bate vira");

        s.push({"run", {{"input", {"a", "b"}}, {"throw_on", 3}}});
        checkError(s, "Throw on token 3");
    }
}

TEST_CASE("general cutoff") {
    DummyRegistry d;

    auto s = createTestSession(d);

    s.push({ "load", {{"file_path", AC_DUMMY_MODEL_SMALL}} });
    CHECK(s.poll().blocked());

    s.push({ "create", {{"cutoff", 2}} });
    CHECK(s.poll().blocked());

    s.push({ "run", {{"input", {"a", "b", "c"}}} });
    checkRunResult(s, "a soco b bate c soco");
}

TEST_CASE("synthetic") {
    DummyRegistry d;
    auto s = createTestSession(d);

    s.push({"load", {}});
    CHECK(s.poll().blocked());

    s.push({"create", {}});
    CHECK(s.poll().blocked());

    s.push({"run", {{"input", {"a", "b"}}}});
    checkRunResult(s, "a one b two");
}
