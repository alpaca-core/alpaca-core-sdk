// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <doctest/doctest.h>
#include <vector>

#include "ac/llama/AntipromptManager.hpp"

TEST_CASE("incremental finder - empty") {
    // by default string finder has empty search string
    // and will always return false
    ac::llama::IncrementalStringFinder f;
    CHECK(f.feedText("") == false);
    CHECK(f.feedText("empty") == false);

    f = ac::llama::IncrementalStringFinder("demo");
    // empty feed
    CHECK(f.feedText("") == false);
}

TEST_CASE("incremental finder - partial match") {
    ac::llama::IncrementalStringFinder f("demo");
    CHECK_FALSE(f.feedText("de"));
    CHECK(f.feedText("mo"));

    f = ac::llama::IncrementalStringFinder("the");
    // no match
    CHECK_FALSE(f.feedText("empty"));

    // complex partial match
    CHECK_FALSE(f.feedText("emptyth")); // last 2 are 'th'
    CHECK(f.feedText("ehooooo")); // + 'e' from the start
}

TEST_CASE("incremental finder - substring") {
    ac::llama::IncrementalStringFinder f("demo");
    // complex substring
    CHECK_FALSE(f.feedText("dede")); // will find only 2
    CHECK(f.feedText("demo2")); // has the contaning string
}

TEST_CASE("incremental finder - case sensitivity") {
    // case sensitivity
    ac::llama::IncrementalStringFinder f("The");
    CHECK_FALSE(f.feedText("the"));
}

TEST_CASE("antiprompt manager - empty") {
    ac::llama::AntipromptManager am;
    am.addAntiprompt("");
    CHECK_FALSE(am.feedGeneratedText("empty"));

    am.addAntiprompt("user:");
    CHECK_FALSE(am.feedGeneratedText(""));
}

TEST_CASE("antiprompt manager - detect") {
    ac::llama::AntipromptManager am;
    am.addAntiprompt("exit");
    am.addAntiprompt("quit");
    CHECK_FALSE(am.feedGeneratedText("please continue"));
    CHECK(am.feedGeneratedText("please exit!"));
    CHECK(am.feedGeneratedText("please quit now!"));
}

TEST_CASE("antiprompt manager - incremental feed") {
    ac::llama::AntipromptManager am;
    am.addAntiprompt("downstream");
    am.addAntiprompt("shutdown");

    CHECK_FALSE(am.feedGeneratedText("shut"));   // Partial match, so false
    CHECK(am.feedGeneratedText("down"));    // Completes the match, so true

    CHECK_FALSE(am.feedGeneratedText("stream")); // state should be reset after match
}

TEST_CASE("antiprompt manager - reset/clear") {
    ac::llama::AntipromptManager am;
    am.addAntiprompt("cancel");

    CHECK_FALSE(am.feedGeneratedText("cance"));  // Partial match, so false
    am.reset();  // Reset the manager's antiprompts state
    CHECK(am.feedGeneratedText("cancel")); // Should match, since the state was reset

    am.clear();  // Clear the manager's antiprompts
    CHECK_FALSE(am.feedGeneratedText("cancel")); // Should match, since the state was reset

    am.addAntiprompt("cancel");// add the antiprompt again
    CHECK(am.feedGeneratedText("cancel"));
}
