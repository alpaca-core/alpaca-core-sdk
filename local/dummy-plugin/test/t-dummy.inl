// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

// inline file
// needs a definition of DummyRegistry to be complete

#include "ac-test-data-dummy-models.h"

#include <ac/local/SyncBackend.hpp>
#include <ac/local/fs/FileUri.hpp>
#include <ac/schema/FrameHelpers.hpp>
#include <ac/schema/StateChange.hpp>
#include <ac/schema/Error.hpp>
#include <ac/schema/Abort.hpp>
#include <ac/schema/Progress.hpp>

#include <doctest/doctest.h>

#include <astl/move.hpp>

ac::local::SyncBackend backend("dummy-test");

using Session = ac::local::SyncBackend::Io;

ac::Dict makeAssetsDict(std::string_view path) {
    return ac::Dict::array({
        {{"tag", "model"}, {"uri", ac::local::fs::FileUri_fromPath(path)}}
    });
}

Session createTestSession() {
    return backend.connect("dummy", {});
}

void pollProgress(Session& s, int n) {
    for (int i = 0; i < n; ++i) {
        auto res = s.get();
        CHECK(res.success());
        auto prog = Frame_optTo(ac::schema::Progress{}, res.value);
        REQUIRE(prog);
    }
}

void checkError(Session& s, const std::string_view msg) {
    auto res = s.get();
    CHECK(res.success());
    auto err = Frame_optTo(ac::schema::Error{}, res.value);
    CHECK(err == msg);
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
    CHECK(Frame_optTo(ac::schema::StateChange{}, res.value) == expectedState);
}

TEST_CASE("bad model") {
    auto s = createTestSession();
    checkStateChange(s, "dummy");

    s.put({"nope", {}});
    checkError(s, "dummy: unknown op: nope");

    s.put({"load_model", {{"assets", makeAssetsDict("/nope.txt")}}});
    pollProgress(s, 1);
    checkError(s, "asset-mgr: file not found: /nope.txt");
}

TEST_CASE("bad instance") {
    auto s = createTestSession();

    checkStateChange(s, "dummy");

    s.put({"load_model", {{"assets", makeAssetsDict(AC_DUMMY_MODEL_SMALL)}}});
    pollProgress(s, 1);
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

        s.put({"load_model", {{"assets", makeAssetsDict(AC_DUMMY_MODEL_SMALL)}}});
        pollProgress(s, 1);
        checkStateChange(s, "model_loaded");

        s.put({"create_instance", {}});
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

        s.put({"load_model", {{"assets", makeAssetsDict(AC_DUMMY_MODEL_SMALL)}}});
        pollProgress(s, 1);
        checkStateChange(s, "model_loaded");

        s.put({"create_instance", {{"cutoff", 2}}});
        checkStateChange(s, "instance");

        s.put({ "run", {{"input", {"a", "b", "c"}}} });
        checkRunResult(s, "a soco b bate c soco");
    }

    // synthetic
    {
        auto s = createTestSession();

        checkStateChange(s, "dummy");

        s.put({"load_model", {}});
        pollProgress(s, 1);
        checkStateChange(s, "model_loaded");

        s.put({"create_instance", {}});
        checkStateChange(s, "instance");

        s.put({"run", {{"input", {"a", "b"}}}});
        checkRunResult(s, "a one b two");
    }

    // stream
    {
        auto s = createTestSession();

        checkStateChange(s, "dummy");

        s.put({"load_model", {{"assets", makeAssetsDict(AC_DUMMY_MODEL_SMALL)}}});
        pollProgress(s, 1);
        checkStateChange(s, "model_loaded");

        s.put({ "create_instance", {} });
        checkStateChange(s, "instance");

        s.put({"stream", {{"input", {"a", "b"}}}});

        auto f = s.get();
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

        f = s.get();
        CHECK(f.success());
        CHECK(f.value.op == "stream");

        s.put({"stream", {{"input", {"x", "y"}}}});

        f = s.get();
        CHECK(f.success());
        CHECK(f.value.op == "token");
        CHECK(f.value.data.get<std::string>() == "x");

        s.put(Frame_from(ac::schema::Abort{}, {}));

        f = s.get();
        CHECK(f.success());
        CHECK(f.value.op == "token");
        CHECK(f.value.data.get<std::string>() == "soco");

        f = s.get();
        CHECK(f.success());
        CHECK(f.value.op == "stream");
    }
}
