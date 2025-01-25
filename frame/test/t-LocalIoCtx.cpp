// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/frameio/local/LocalBufferedChannel.hpp>
#include <ac/frameio/local/LocalChannelUtil.hpp>
#include <ac/frameio/local/LocalIoCtx.hpp>
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

SessionCoro<void> sideA() {
    for (int i = 1; i < 10; ++i) {
        auto res = co_await coro::pollFrame(no_wait);
        if (res.success()) {
            A_recSum += std::stoi(res.frame.op);
        }
        co_await coro::pushFrame(frame(std::to_string(i)));
    }

    co_await coro::pushFrame(frame("end"));

    try {
        while (true) {
            auto res = co_await coro::pollFrame();
            if (res.success()) {
                A_recSum += std::stoi(res.frame.op);
            }
        }
    }
    catch (const coro::IoClosed&) {
        A_done = true;
    }
}

int B_recSum = 0;

SessionCoro<void> sideB() {
    int send = 15;
    bool receive = true;

    while (send || receive) {
        if (send) {
            co_await coro::pushFrame(frame(std::to_string(1000 + send)));
            --send;
        }
        if (receive) {
            auto f = co_await coro::pollFrame(await_completion_for(20ms));
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
    LocalIoCtx ctx;

    auto run = [&ctx]() {
        ctx.run();
    };
    std::thread a(run), b(run);

    auto [elocal, eremote] = LocalChannel_getEndpoints(
        LocalBufferedChannel_create(5),
        LocalBufferedChannel_create(5)
    );

    ctx.connect(CoroSessionHandler::create(sideA()), std::move(elocal));
    ctx.connect(CoroSessionHandler::create(sideB()), std::move(eremote));

    ctx.complete();
    a.join();
    b.join();

    CHECK(A_recSum == 15120);
    CHECK(A_done);
    CHECK(B_recSum == 45);
}