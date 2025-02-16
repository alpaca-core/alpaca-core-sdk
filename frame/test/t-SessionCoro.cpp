// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/frameio/local/BlockingSyncIoWrapper.hpp>
#include <ac/frameio/SessionHandler.hpp>
#include <ac/frameio/SessionCoro.hpp>
#include <ac/frameio/StreamEndpoint.hpp>
#include <ac/frameio/local/LocalIoRunner.hpp>
#include <ac/frameio/local/BlockingIo.hpp>
#include <doctest/doctest.h>
#include <string>

using namespace ac::frameio;

SessionCoro<std::string> add(coro::Io& io) {
    auto fin = co_await io.pollFrame();
    auto a = std::stoi(fin.value.op);
    fin = co_await io.pollFrame();
    auto b = std::stoi(fin.value.op);
    co_return std::to_string(a + b);
}

SessionCoro<void> session() {
    auto io = co_await coro::Io{};
    while (true) {
        auto fin = co_await io.pollFrame();
        std::string result;
        if (fin.value.op == "echo") {
            result = fin.value.op;
        }
        else if (fin.value.op == "add") {
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

ac::Frame frame(std::string str, ac::Dict data = {}) {
    return ac::Frame{str, std::move(data)};
}

void testSession(BlockingSyncIoWrapper& io) {
    io.push(frame("echo"));
    auto f = io.poll();
    CHECK(f.success());
    CHECK(f.value.op == "echo");

    io.push(frame("add"));
    io.push(frame("1"));
    io.push(frame("2"));
    f = io.poll();
    CHECK(f.success());
    CHECK(f.value.op == "3");

    io.push(frame("add"));
    io.push(frame("3"));
    io.push(frame("4"));
    f = io.poll();
    CHECK(f.success());
    CHECK(f.value.op == "7");

    io.push(frame("echo"));
    f = io.poll();
    CHECK(f.success());
    CHECK(f.value.op == "echo");

    io.push(frame("nope"));
    f = io.poll();
    CHECK(f.closed());
}

TEST_CASE("SessionCoro") {
    BlockingSyncIoWrapper io(CoroSessionHandler::create(session()));
    testSession(io);
}

SessionCoro<SessionHandlerPtr> successorSession() {
    auto io = co_await coro::Io{};
    int i = 0;
    while (true) {
        auto fin = co_await io.pollFrame();
        auto& op = fin.value.op;
        if (op == "goto echo") {
            co_return CoroSessionHandler::create(session());
        }
        else if (op == "i") {
            co_await io.pushFrame(frame(std::to_string(i++)));
        }
        else {
            co_return nullptr;
        }
    }
}

TEST_CASE("successor session") {
    {
        BlockingSyncIoWrapper io(CoroSessionHandler::create(successorSession()));
        io.push(frame("i"));
        auto f = io.poll();
        CHECK(f.success());
        CHECK(f.value.op == "0");

        io.push(frame("i"));
        f = io.poll();
        CHECK(f.success());
        CHECK(f.value.op == "1");

        io.push(frame("nope"));
        f = io.poll();
        CHECK(f.closed());
    }

    {
        BlockingSyncIoWrapper io(CoroSessionHandler::create(successorSession()));
        io.push(frame("i"));
        auto f = io.poll();
        CHECK(f.success());
        CHECK(f.value.op == "0");

        io.push(frame("i"));
        f = io.poll();
        CHECK(f.success());
        CHECK(f.value.op == "1");

        io.push(frame("goto echo"));
        testSession(io);
    }
}

SessionCoro<void> proxy(StreamEndpoint ep) {
    auto client = co_await coro::Io{};
    auto server = client.attach(std::move(ep));

    while (true) {
        auto fin = co_await client.pollFrame();
        if (fin.value.op == "done") {
            co_return;
        }
        fin.value.op += " (proxied)";
        co_await server.pushFrame(fin.value);
        auto fout = co_await server.pollFrame();
        co_await client.pushFrame(fout.value);
    }
}

SessionCoro<void> server() {
    auto io = co_await coro::Io{};
    while (true) try {
        auto fin = co_await io.pollFrame();
        auto i = fin.value.data.get<int>();
        ac::Frame fout;
        fout.op = "ret";
        fout.data = i + 1;
        co_await io.pushFrame(fout);
    }
    catch (IoClosed&) {
        co_return;
    }
}

TEST_CASE("proxy") {
    LocalIoRunner ctx;
    auto ep = ctx.connect(CoroSessionHandler::create(server()));
    BlockingIo io(ctx.connect(CoroSessionHandler::create(proxy(std::move(ep)))));

    io.push(frame("xx", 1));
    auto f = io.poll();
    CHECK(f.success());
    CHECK(f.value.op == "ret");
    CHECK(f.value.data.get<int>() == 2);

    io.push(frame("yy", 32));
    f = io.poll();
    CHECK(f.success());
    CHECK(f.value.op == "ret");
    CHECK(f.value.data.get<int>() == 33);

    io.push(frame("done"));
    f = io.poll();
    CHECK(f.closed());
}
