// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

// inline file
// needs a definition of DummyRegistry to be complete

#include "ac-test-data-dummy-models.h"

#include <ac/local/Model.hpp>
#include <ac/local/Instance.hpp>

#include <ac/local/SyncSession.hpp>

#include <doctest/doctest.h>

#include <astl/move.hpp>

const ac::local::ModelAssetDesc Model_Desc = {
    .type = "dummy",
    .assets = {
        {.path = AC_DUMMY_MODEL_SMALL}
    }
};

std::unique_ptr<ac::local::SyncSession> createTestSession(DummyRegistry& d) {
    REQUIRE(d.providers().size() == 1);
    auto s = std::make_unique<ac::local::SyncSession>(d.providers().front().provider->createSessionHandler({}));
    REQUIRE(s);
    REQUIRE(s->valid());
    return s;
}

void checkError(ac::local::SyncSession& s, const std::string_view msg) {
    auto frame = s.get();
    REQUIRE(frame);
    CHECK(frame->op == "error");
    CHECK(frame->data.get<std::string_view>() == msg);
}

TEST_CASE("bad model") {
    DummyRegistry d;
    auto s = createTestSession(d);
    CHECK_FALSE(s->get());
    s->put({"nope", {}});
    checkError(*s, "dummy: expected 'load' op, got: nope");

    s->put({"load", {{"file_path", "nope"}}});
    checkError(*s, "Failed to open file: nope");
}

//TEST_CASE("bad model") {
//    DummyRegistry f;
//    CHECK_THROWS_WITH(
//        f.loadModel({
//            .type = "dummy",
//            .assets = {
//                {.path = "nope"}
//            }
//        }, {}),
//        "Failed to open file: nope"
//    );
//}

TEST_CASE("bad instance") {
    DummyRegistry d;
    auto s = createTestSession(d);

    s->put({ "load", {{"file_path", AC_DUMMY_MODEL_SMALL}} });
    CHECK_FALSE(s->get());

    s->put({ "nope", {} });
    checkError(*s, "dummy: expected 'create' op, got: nope");

    s->put({ "create", {{"cutoff", 40}} });
    checkError(*s, "Cutoff 40 greater than model size 3");
}

TEST_CASE("general") {
    DummyRegistry d;
    auto s = createTestSession(d);

    s->put({"load", {{"file_path", AC_DUMMY_MODEL_SMALL}}});
    CHECK_FALSE(s->get());

    s->put({"create", {}});
    CHECK_FALSE(s->get());

    s->put({"nope", {}});
    checkError(*s, "dummy: unknown op: nope");

    s->put({ "run", {{"foo", "nope"}}});
    checkError(*s, "Required field input is not set");

    s->put({"run", {{"input", {"a", "b"}}}});
    auto f = s->get();
    REQUIRE(f);
    CHECK(f->op == "run");
    CHECK(f->data.at("result").get<std::string>() == "a soco b bate");

    s->put({"run", {{"input", {"a", "b"}}, {"splice", false}}});
    f = s->get();
    REQUIRE(f);
    CHECK(f->op == "run");
    CHECK(f->data.at("result").get<std::string>() == "a b soco bate vira");

    s->put({"run", {{"input", {"a", "b"}}, {"throw_on", 3}}});
    checkError(*s, "Throw on token 3");

    auto s2 = createTestSession(d);

    s2->put({"load", {{"file_path", AC_DUMMY_MODEL_SMALL}}});
    CHECK_FALSE(s2->get());

    s2->put({"create", {{"cutoff", 2}}});
    CHECK_FALSE(s2->get());

    s2->put({"run", {{"input", {"a", "b", "c"}}}});
    f = s2->get();
    REQUIRE(f);
    CHECK(f->op == "run");
    CHECK(f->data.at("result").get<std::string>() == "a soco b bate c soco");
}

TEST_CASE("synthetic") {
    DummyRegistry d;
    auto s = createTestSession(d);

    s->put({"load", {}});
    CHECK_FALSE(s->get());

    s->put({"create", {}});
    CHECK_FALSE(s->get());

    s->put({"run", {{"input", {"a", "b"}}}});
    auto f = s->get();
    REQUIRE(f);
    CHECK(f->op == "run");
    CHECK(f->data.at("result").get<std::string>() == "a one b two");;
}
