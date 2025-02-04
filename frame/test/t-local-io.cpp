// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/frameio/local/LocalIoRunner.hpp>
#include <ac/frameio/local/BlockingIo.hpp>
#include <ac/frameio/local/LocalChannelUtil.hpp>
#include <ac/frameio/SessionCoro.hpp>
#include <doctest/doctest.h>
#include <thread>

using namespace ac::frameio;
using namespace astl::timeout_vals;
using namespace std::chrono_literals;

ac::Frame frame(std::string str) {
    return ac::Frame{str, {}};
}

int A_recSum = 0;
bool A_done = true;

SessionCoro<void> sideA(int send) {
    auto io = co_await coro::Io{};

    for (int i = 1; i <= send; ++i) {
        auto res = co_await io.pollFrame(no_wait);
        if (res.success()) {
            A_recSum += std::stoi(res.frame.op);
        }
        co_await io.pushFrame(frame(std::to_string(i)));
    }

    co_await io.pushFrame(frame("end"));

    try {
        while (true) {
            auto res = co_await io.pollFrame();
            if (res.success()) {
                A_recSum += std::stoi(res.frame.op);
            }
        }
    }
    catch (const IoClosed&) {
        A_done = true;
    }
}

int B_recSum = 0;

SessionCoro<void> sideB(int send) {
    bool receive = true;

    auto io = co_await coro::Io{};

    while (send || receive) {
        if (send) {
            co_await io.pushFrame(frame(std::to_string(1000 + send)));
            --send;
        }
        if (receive) {
            auto f = co_await io.pollFrame(await_completion_for(20ms));
            if (f.success()) {
                if (f.frame.op == "end") {
                    receive = false;
                }
                else {
                    B_recSum += std::stoi(f.frame.op);
                }
            }
        }
    }
}

TEST_CASE("local io") {
    LocalIoRunner runner(2);

    const int asend = 10, bsend = 15;
    runner.connect(CoroSessionHandler::create(sideA(asend)), CoroSessionHandler::create(sideB(bsend)));

    runner.join();

    CHECK(A_recSum == 1000 * bsend + (bsend * (bsend + 1)) / 2);
    CHECK(A_done);
    CHECK(B_recSum == (asend * (asend + 1)) / 2);
}

SessionCoro<void> eagerSession() {
    auto io = co_await coro::Io{};

    auto fin = io.getFrame();
    CHECK(fin.success());
    auto i = std::stoi(fin.frame.op);
    while (i) {
        auto f = frame("hi");
        while (!io.putFrame(f).success()); // spin
        --i;
    }
}

TEST_CASE("eager") {
    LocalIoRunner runner(1);

    auto [elocal, eremote] = LocalIoRunner::getEndpoints({3, 3});

    BlockingIo localIo(std::move(elocal));

    localIo.push(frame("10"));

    runner.ctx().connect(CoroSessionHandler::create(eagerSession()), std::move(eremote));

    int received = 0;
    while (true) {
        auto f = localIo.poll();
        if (!f.success()) break;
        CHECK(f.frame.op == "hi");
        ++received;
    }
    CHECK(received == 10);
}
