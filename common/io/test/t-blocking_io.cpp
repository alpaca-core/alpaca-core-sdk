// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/io/blocking_io.hpp>
#include <ac/io/buffered_channel_endpoints.hpp>
#include <ac/io/channel_stream.hpp>
#include <ac/io/channel_endpoints.hpp>
#include <doctest/doctest.h>

TEST_CASE("blocking_io") {
    using namespace astl::timeout_vals;
    using namespace std::chrono_literals;

    auto [elocal, eremote] = ac::io::make_buffered_channel_endpoints<std::string>(3, 3);

    ac::io::blocking_io_ctx ctx;

    ac::io::blocking_io local(std::move(elocal), ctx);
    ac::io::blocking_io remote(std::move(eremote), ctx);

    CHECK(local.push("a").success());
    CHECK(local.push("b", no_wait).success());
    CHECK(local.push("c", await_completion_for(20ms)).success());
    auto status = local.push("d", no_wait);

    CHECK(status.timeout());
    CHECK(status.aborted());
    CHECK_FALSE(status.waiting());

    auto f = remote.poll();
    CHECK(f.success());
    CHECK(f.value == "a");
    f = remote.poll();
    CHECK(f.success());
    CHECK(f.value == "b");
    f = remote.poll();
    CHECK(f.success());
    CHECK(f.value == "c");
    f = remote.poll(await_completion_for(10ms));
    CHECK(f.value.empty());
    CHECK(f.timeout());
    CHECK(f.aborted());
    CHECK_FALSE(f.waiting());
}