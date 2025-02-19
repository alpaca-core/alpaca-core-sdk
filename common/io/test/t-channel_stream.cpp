// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/io/channel_stream.hpp>
#include <ac/io/stream_result.hpp>
#include <ac/io/channel_endpoints.hpp>
#include <doctest/doctest.h>

struct test_channel {
    using read_value_type = int;
    using write_value_type = int;

    int val = 0;

    ac::io::stream_result write(int& t, ac::xec::task(*)()) {
        val = t;
        t = 0;
        return ac::io::stream_result::build().set_success();
    }

    ac::io::stream_result read(int& t, ac::xec::task(*)()) {
        t = val;
        val = 0;
        return ac::io::stream_result::build().set_success();
    }

    void close() {
        val = -1;
    }
};

TEST_CASE("channel_stream") {
    auto [ab, ba] = make_channel_endpoints(
        ac::io::make_channel_streams(std::make_unique<test_channel>()),
        ac::io::make_channel_streams(std::make_unique<test_channel>())
    );
    int r = 6, w = 0;

    ab.read_stream->read(r, nullptr);
    CHECK(r == 0);

    r = 12;
    ba.read_stream->read(r, nullptr);
    CHECK(r == 0);

    w = 543;
    ab.write_stream->write(w, nullptr);

    ba.read_stream->read(r, nullptr);
    CHECK(r == 543);

    ba.read_stream->read(r, nullptr);
    CHECK(r == 0);

    w = 32;
    ba.write_stream->write(w, nullptr);

    ab.read_stream->read(r, nullptr);
    CHECK(r == 32);

    ab.write_stream->close();
    ba.read_stream->read(r, nullptr);
    CHECK(r == -1);
}
