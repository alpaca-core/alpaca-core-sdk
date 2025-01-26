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
    catch (const coro::IoClosed&) {
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
    LocalIoCtx ctx;

    auto run = [&ctx]() {
        ctx.run();
    };
    std::thread a(run), b(run);

    auto [elocal, eremote] = LocalChannel_getEndpoints(
        LocalBufferedChannel_create(5),
        LocalBufferedChannel_create(5)
    );

    const int asend = 10, bsend = 15;

    ctx.connect(CoroSessionHandler::create(sideA(asend)), std::move(elocal));
    ctx.connect(CoroSessionHandler::create(sideB(bsend)), std::move(eremote));

    ctx.complete();
    a.join();
    b.join();

    CHECK(A_recSum == 1000 * bsend + (bsend * (bsend + 1)) / 2);
    CHECK(A_done);
    CHECK(B_recSum == (asend * (asend + 1)) / 2);
}