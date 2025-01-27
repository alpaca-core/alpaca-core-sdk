// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/frameio/local/BlockingSyncIoWrapper.hpp>
#include <ac/frameio/SessionHandler.hpp>
#include <ac/frameio/SessionCoro.hpp>
#include <doctest/doctest.h>
#include <string>

using namespace ac::frameio;

SessionCoro<std::string> add(coro::Io& io) {
    auto fin = co_await io.pollFrame();
    auto a = std::stoi(fin.frame.op);
    fin = co_await io.pollFrame();
    auto b = std::stoi(fin.frame.op);
    co_return std::to_string(a + b);
}

SessionCoro<void> session() {
    auto io = co_await coro::Io{};
    while (true) {
        auto fin = co_await io.pollFrame();
        std::string result;
        if (fin.frame.op == "echo") {
            result = fin.frame.op;
        }
        else if (fin.frame.op == "add") {
            result = co_await add(io);
        }
        else {
            co_return;
        }

        ac::Frame fout;
        fout.op = result;
        co_await io.pushFrame(fout);
    }
}

ac::Frame frame(std::string str) {
    return ac::Frame{ str, {} };
}

TEST_CASE("SessionCoro") {
    BlockingSyncIoWrapper io(CoroSessionHandler::create(session()));

    io.push(frame("echo"));
    auto f = io.poll();
    CHECK(f.success());
    CHECK(f.frame.op == "echo");

    io.push(frame("add"));
    io.push(frame("1"));
    io.push(frame("2"));
    f = io.poll();
    CHECK(f.success());
    CHECK(f.frame.op == "3");

    io.push(frame("add"));
    io.push(frame("3"));
    io.push(frame("4"));
    f = io.poll();
    CHECK(f.success());
    CHECK(f.frame.op == "7");

    io.push(frame("echo"));
    f = io.poll();
    CHECK(f.success());
    CHECK(f.frame.op == "echo");

    io.push(frame("nope"));
    f = io.poll();
    CHECK(f.closed());
}
