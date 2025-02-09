// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

// inline file
// needs a definition of DummyRegistry to be complete

#include "ac-test-data-dummy-models.h"

#include <ac/frameio/local/BlockingSyncIoWrapper.hpp>
#include <ac/FrameUtil.hpp>

#include <doctest/doctest.h>

#include <astl/move.hpp>

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
}

void checkRunResult(Session& s, const std::string_view msg) {
    auto res = s.poll();
    CHECK(res.success());
    auto& frame = res.frame;
    CHECK(frame.op == "run");
    CHECK(frame.data.at("result").get<std::string>() == msg);
}

void checkStateChange(Session& s, std::string_view expectedState) {
    auto res = s.poll();
    CHECK(res.success());
    CHECK(Frame_isStateChange(res.frame));
    CHECK(Frame_getStateChange(res.frame) == expectedState);
}

TEST_CASE("bad model") {
    DummyRegistry d;

    auto s = createTestSession(d);
    checkStateChange(s, "dummy");

    s.push({"nope", {}});
    checkError(s, "dummy: unknown op: nope");

    s.push({"load_model", {{"file_path", "nope"}}});
    checkError(s, "Failed to open file: nope");
}

TEST_CASE("bad instance") {
    DummyRegistry d;
    auto s = createTestSession(d);

    checkStateChange(s, "dummy");

    s.push({"load_model", {{"file_path", AC_DUMMY_MODEL_SMALL}}});
    CHECK(s.poll().success());
    checkStateChange(s, "model_loaded");

    s.push({"nope", {}});
    checkError(s, "dummy: unknown op: nope");

    s.push({"create_instance", {{"cutoff", 40}}});
    checkError(s, "Cutoff 40 greater than model size 3");
}

TEST_CASE("instance") {
    DummyRegistry d;

    // basic
    {
        auto s = createTestSession(d);

        checkStateChange(s, "dummy");

        s.push({"load_model", {{"file_path", AC_DUMMY_MODEL_SMALL}}});
        CHECK(s.poll().success());
        checkStateChange(s, "model_loaded");

        s.push({"create_instance", {}});
        CHECK(s.poll().success());
        checkStateChange(s, "instance");

        s.push({"nope", {}});
        checkError(s, "dummy: unknown op: nope");

        s.push({ "run", {{"foo", "nope"}} });
        checkError(s, "Required field input is not set");

        s.push({"run", {{"input", {"a", "b"}}}});
        checkRunResult(s, "a soco b bate");

        s.push({"run", {{"input", {"a", "b"}}, {"splice", false}}});
        checkRunResult(s, "a b soco bate vira");

        s.push({"run", {{"input", {"a", "b"}}, {"throw_on", 3}}});
        checkError(s, "Throw on token 3");
    }

    // cutoff
    {
        auto s = createTestSession(d);

        checkStateChange(s, "dummy");

        s.push({"load_model", {{"file_path", AC_DUMMY_MODEL_SMALL}}});
        CHECK(s.poll().success());
        checkStateChange(s, "model_loaded");

        s.push({"create_instance", {{"cutoff", 2}}});
        CHECK(s.poll().success());
        checkStateChange(s, "instance");

        s.push({ "run", {{"input", {"a", "b", "c"}}} });
        checkRunResult(s, "a soco b bate c soco");
    }

    // synthetic
    {
        auto s = createTestSession(d);

        checkStateChange(s, "dummy");

        s.push({"load_model", {}});
        CHECK(s.poll().success());
        checkStateChange(s, "model_loaded");

        s.push({"create_instance", {}});
        CHECK(s.poll().success());
        checkStateChange(s, "instance");

        s.push({"run", {{"input", {"a", "b"}}}});
        checkRunResult(s, "a one b two");
    }
}
