// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/dummy/Model.hpp>
#include "ac-test-data-dummy-models.h"
#include <doctest/doctest.h>
#include <algorithm>

TEST_CASE("simple") {
    ac::dummy::Model model(AC_DUMMY_MODEL_LARGE, {});
    auto& data = model.data();
    CHECK(data.size() == 25);
    CHECK(data[0] == "We");
    CHECK(data[1] == "choose");
    CHECK(data[2] == "to");
    CHECK(data[3] == "go");
    CHECK(data[4] == "to");

    CHECK(data[23] == "are");
    CHECK(data[24] == "hard");
}

TEST_CASE("splice") {
    ac::dummy::Model model(AC_DUMMY_MODEL_SMALL, {.splice = "Soco"});
    auto& data = model.data();
    CHECK(data.size() == 6);
    CHECK(data[0] == "Soco");
    CHECK(data[1] == "soco");
    CHECK(data[2] == "Soco");
    CHECK(data[3] == "bate");
    CHECK(data[4] == "Soco");
    CHECK(data[5] == "vira");
}

TEST_CASE("synthetic") {
    ac::dummy::Model model({});
    auto& data = model.data();

    auto rawData = ac::dummy::Model::rawSyntheticModelData();
    CHECK(std::equal(data.begin(), data.end(), rawData.begin(), rawData.end()));
}

TEST_CASE("exceptions") {
    CHECK_THROWS_WITH_AS(ac::dummy::Model("nope nope", {}), "Failed to open file: nope nope", std::runtime_error);
}
