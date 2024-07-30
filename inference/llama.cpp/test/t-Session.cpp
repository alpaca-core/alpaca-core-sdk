// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/llama/Session.hpp>
#include <doctest/doctest.h>
#include <vector>
#include <deque>

// the session is a coroutine wrapper
// we can test it in isolation with a fake instance (no actual AI inference going on)
// in this case we create a coroutine which:
// * receives an initial prompt
// * for every awaited additional prompt, it splits it into words and yields the length of the initial prompt and then
//   the length of each word
std::vector<std::string> splitToWords(std::string_view text) {
    std::vector<std::string> ret;

    while (true) {
        auto pos = text.find(' ');
        if (pos == std::string::npos) {
            ret.push_back(std::string(text));
            break;
        }
        ret.push_back(std::string(text.substr(0, pos)));
        text = text.substr(pos + 1);
    }

    return ret;
}

ac::llama::Session TestSession(std::string initialPrompt) {
    const auto initial = splitToWords(initialPrompt);

    std::deque<std::string> current(initial.begin(), initial.end());

    bool yieldedInvalid = false;

    while (true) {
        auto& prompt = co_await ac::llama::Session::Prompt{};

        if (prompt.empty()) {
            if (current.empty() && yieldedInvalid) {
                current.push_back("ok one more");
                yieldedInvalid = false;
            }
        }
        else {
            current.assign(initial.begin(), initial.end());
            auto split = splitToWords(prompt);
            current.insert(current.end(), split.begin(), split.end());
            yieldedInvalid = false;
        }

        if (current.empty()) {
            co_yield ac::llama::Token_Invalid;
            yieldedInvalid = true;
        }
        else {
            co_yield ac::llama::Token(current.front().size());
            current.pop_front();
        }
    }
}


TEST_CASE("split") {
    // test the test code
    CHECK(splitToWords("the sky is blue") == std::vector<std::string>{"the", "sky", "is", "blue"});
    CHECK(splitToWords("42") == std::vector<std::string>{"42"});
    CHECK(splitToWords("") == std::vector<std::string>{""});
}

TEST_CASE("session") {
    {
        auto session = TestSession("i am");
        CHECK(session.getToken() == 1);
        CHECK(session.getToken() == 2);
        CHECK(session.getToken() == ac::llama::Token_Invalid);
        CHECK(session.getToken() == 11);
        CHECK(session.getToken() == ac::llama::Token_Invalid);

        session.pushPrompt("red panda");
        CHECK(session.getToken() == 1);
        CHECK(session.getToken() == 2);
        CHECK(session.getToken() == 3);
        CHECK(session.getToken() == 5);
        CHECK(session.getToken() == ac::llama::Token_Invalid);
        CHECK(session.getToken() == 11);
        CHECK(session.getToken() == ac::llama::Token_Invalid);

        session.pushPrompt("green hornet");
        CHECK(session.getToken() == 1);
        CHECK(session.getToken() == 2);
        CHECK(session.getToken() == 5);

        session.pushPrompt("blue whale");
        CHECK(session.getToken() == 1);
        CHECK(session.getToken() == 2);
        CHECK(session.getToken() == 4);
    }

    {
        auto session = TestSession("he isn't");
        session.pushPrompt("a dog");
        CHECK(session.getToken() == 2);
        CHECK(session.getToken() == 5);
        CHECK(session.getToken() == 1);
        CHECK(session.getToken() == 3);
        CHECK(session.getToken() == ac::llama::Token_Invalid);
        CHECK(session.getToken() == 11);
        CHECK(session.getToken() == ac::llama::Token_Invalid);
    }
}