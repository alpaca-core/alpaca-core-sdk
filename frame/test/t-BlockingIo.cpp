// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/frameio/local/LocalBufferedChannel.hpp>
#include <ac/frameio/local/LocalChannelUtil.hpp>
#include <ac/frameio/local/BlockingIo.hpp>
#include <doctest/doctest.h>

using namespace ac::frameio;
using namespace astl::timeout_vals;
using namespace std::chrono_literals;

ac::Frame frame(std::string str) {
    return ac::Frame{str, {}};
}

TEST_CASE("BlockingIo") {
    auto [elocal, eremote] = LocalChannel_getEndpoints(
        LocalBufferedChannel_create(3),
        LocalBufferedChannel_create(3)
    );

    BlockingIo local(std::move(elocal));
    BlockingIo remote(std::move(eremote));

    CHECK(local.push(frame("a")).success());
    CHECK(local.push(frame("b"), no_wait).success());
    CHECK(local.push(frame("c"), await_completion_for(20ms)).success());
    auto status = local.push(frame("d"), no_wait);
    CHECK(status.timeout());
    CHECK(status.aborted());
    CHECK_FALSE(status.waiting());

    auto f = remote.poll();
    CHECK(f.success());
    CHECK(f.value.op == "a");
    f = remote.poll();
    CHECK(f.success());
    CHECK(f.value.op == "b");
    f = remote.poll();
    CHECK(f.success());
    CHECK(f.value.op == "c");
    f = remote.poll(await_completion_for(10ms));
    CHECK(f.value.op.empty());
    CHECK(f.timeout());
    CHECK(f.aborted());
    CHECK_FALSE(f.waiting());
}
