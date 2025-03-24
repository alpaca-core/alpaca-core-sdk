// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

// inline file
// needs a definition of DummyRegistry to be complete

#include "ac-test-data-dummy-models.h"

#include <ac/local/SyncBackend.hpp>
#include <ac/FrameUtil.hpp>

#include <doctest/doctest.h>

#include <astl/move.hpp>

ac::local::SyncBackend backend("dummy-test");

using Session = ac::local::SyncBackend::Io;

Session createTestSession() {
    return backend.connect("dummy", {});
}

void checkError(Session& s, const std::string_view msg) {
    auto res = s.get();
    CHECK(res.success());
    auto& frame = res.value;
    CHECK(frame.op == "error");
    CHECK(frame.data.get<std::string_view>() == msg);
}

void checkRunResult(Session& s, const std::string_view msg) {
    auto res = s.get();
    CHECK(res.success());
    auto& frame = res.value;
    CHECK(frame.op == "run");
    CHECK(frame.data.at("result").get<std::string>() == msg);
}

void checkStateChange(Session& s, std::string_view expectedState) {
    auto res = s.get();
    CHECK(res.success());
    CHECK(Frame_isStateChange(res.value));
    CHECK(Frame_getStateChange(res.value) == expectedState);
}

TEST_CASE("bad model") {
    auto s = createTestSession();
    checkStateChange(s, "dummy");

    s.put({"nope", {}});
    checkError(s, "dummy: unknown op: nope");

    s.put({"load_model", {{"file_path", "nope"}}});
    checkError(s, "Failed to open file: nope");
}

TEST_CASE("bad instance") {
    auto s = createTestSession();

    checkStateChange(s, "dummy");

    s.put({"load_model", {{"file_path", AC_DUMMY_MODEL_SMALL}}});
    CHECK(s.get().success());
    checkStateChange(s, "model_loaded");

    s.put({"nope", {}});
    checkError(s, "dummy: unknown op: nope");

    s.put({"create_instance", {{"cutoff", 40}}});
    checkError(s, "Cutoff 40 greater than model size 3");
}

TEST_CASE("instance") {
    // basic
    {
        auto s = createTestSession();

        checkStateChange(s, "dummy");

        s.put({"load_model", {{"file_path", AC_DUMMY_MODEL_SMALL}}});
        CHECK(s.get().success());
        checkStateChange(s, "model_loaded");

        s.put({"create_instance", {}});
        CHECK(s.get().success());
        checkStateChange(s, "instance");

        s.put({"nope", {}});
        checkError(s, "dummy: unknown op: nope");

        s.put({ "run", {{"foo", "nope"}} });
        checkError(s, "Required field input is not set");

        s.put({"run", {{"input", {"a", "b"}}}});
        checkRunResult(s, "a soco b bate");

        s.put({"run", {{"input", {"a", "b"}}, {"splice", false}}});
        checkRunResult(s, "a b soco bate vira");

        s.put({"run", {{"input", {"a", "b"}}, {"throw_on", 3}}});
        checkError(s, "Throw on token 3");
    }

    // cutoff
    {
        auto s = createTestSession();

        checkStateChange(s, "dummy");

        s.put({"load_model", {{"file_path", AC_DUMMY_MODEL_SMALL}}});
        CHECK(s.get().success());
        checkStateChange(s, "model_loaded");

        s.put({"create_instance", {{"cutoff", 2}}});
        CHECK(s.get().success());
        checkStateChange(s, "instance");

        s.put({ "run", {{"input", {"a", "b", "c"}}} });
        checkRunResult(s, "a soco b bate c soco");
    }

    // synthetic
    {
        auto s = createTestSession();

        checkStateChange(s, "dummy");

        s.put({"load_model", {}});
        CHECK(s.get().success());
        checkStateChange(s, "model_loaded");

        s.put({"create_instance", {}});
        CHECK(s.get().success());
        checkStateChange(s, "instance");

        s.put({"run", {{"input", {"a", "b"}}}});
        checkRunResult(s, "a one b two");
    }

    // stream
    {
        auto s = createTestSession();

        checkStateChange(s, "dummy");

        s.put({ "load_model", {{"file_path", AC_DUMMY_MODEL_SMALL}} });
        CHECK(s.get().success());
        checkStateChange(s, "model_loaded");

        s.put({ "create_instance", {} });
        CHECK(s.get().success());
        checkStateChange(s, "instance");

        s.put({"stream", {{"input", {"a", "b"}}}});
        auto f = s.get();
        CHECK(f.success());
        CHECK(f.value.op == "stream");

        checkStateChange(s, "streaming");

        f = s.get();
        CHECK(f.success());
        CHECK(f.value.op == "token");
        CHECK(f.value.data.get<std::string>() == "a");

        f = s.get();
        CHECK(f.success());
        CHECK(f.value.op == "token");
        CHECK(f.value.data.get<std::string>() == "soco");

        f = s.get();
        CHECK(f.success());
        CHECK(f.value.op == "token");
        CHECK(f.value.data.get<std::string>() == "b");

        f = s.get();
        CHECK(f.success());
        CHECK(f.value.op == "token");
        CHECK(f.value.data.get<std::string>() == "bate");

        checkStateChange(s, "instance");

        s.put({"stream", {{"input", {"x", "y"}}}});
        f = s.get();
        CHECK(f.success());
        CHECK(f.value.op == "stream");

        checkStateChange(s, "streaming");

        f = s.get();
        CHECK(f.success());
        CHECK(f.value.op == "token");
        CHECK(f.value.data.get<std::string>() == "x");

        s.put({"abort", {}});

        f = s.get();
        CHECK(f.success());
        CHECK(f.value.op == "token");
        CHECK(f.value.data.get<std::string>() == "soco");

        checkStateChange(s, "instance");
    }
}
