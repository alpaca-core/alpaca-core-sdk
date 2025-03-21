// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/io/buffered_channel.hpp>
#include <ac/io/concepts/channel.hpp>
#include <doctest/doctest.h>
#include <string>

static_assert(ac::io::channel_class<ac::io::buffered_channel<std::string>>);

TEST_CASE("buffered_channel 1") {
    int read_ns = 0, read_ns2 = 0, write_ns = 0;

    std::function<void()>
        read_notify = [&]() {
            ++read_ns;
        },
        read_notify2 = [&]() {
            ++read_ns2;
        },
        write_notify = [&]() {
            ++write_ns;
        }
    ;

    auto on_rb = [&]() { return read_notify; };
    auto on_rb2 = [&]() { return read_notify2; };
    auto on_wb = [&]() { return write_notify; };

    ac::io::buffered_channel<std::string> channel(1);

    auto notify = channel.exchange_read_notify_cb(nullptr);
    CHECK_FALSE(notify);

    notify = channel.exchange_write_notify_cb(nullptr);
    CHECK_FALSE(notify);

    std::string str;
    auto result = channel.read(str, nullptr);
    CHECK(result.blocked());
    CHECK_FALSE(result.value);
    CHECK(str.empty());

    result = channel.read(str, on_rb);
    CHECK_FALSE(result.value);
    CHECK(result.blocked());
    CHECK(str.empty());
    CHECK(read_ns == 0);

    notify = channel.exchange_write_notify_cb(nullptr);
    CHECK_FALSE(notify);

    notify = channel.exchange_read_notify_cb(nullptr);
    REQUIRE(notify);
    notify();
    CHECK(read_ns == 1);

    str = "test";
    result = channel.write(str, nullptr);
    CHECK(result.success());
    CHECK(str.empty());

    str = "nope";
    result = channel.write(str, nullptr);
    CHECK(result.blocked());
    CHECK(str == "nope");

    result = channel.read(str, nullptr);
    CHECK(result.success());
    CHECK(str == "test");

    str.clear();
    result = channel.read(str, on_rb);
    CHECK_FALSE(result.value);
    CHECK(result.blocked());
    CHECK(str.empty());

    str = "yep";
    result = channel.write(str, on_wb);
    CHECK_FALSE(result.value);
    CHECK(result.success());
    CHECK(read_ns == 2);
    CHECK(str.empty());

    str = "nope again";
    result = channel.write(str, on_wb);
    CHECK_FALSE(result.value);
    CHECK(result.blocked());
    CHECK(str == "nope again");

    result = channel.read(str, on_rb);
    CHECK_FALSE(result.value);
    CHECK(result.success());
    CHECK(str == "yep");
    CHECK(read_ns == 2);

    str.clear();
    result = channel.read(str, on_rb);
    CHECK_FALSE(result.value);
    CHECK(result.blocked());
    CHECK(str.empty());

    result = channel.read(str, on_rb2);
    CHECK(result.blocked());
    CHECK(str.empty());
    REQUIRE(result.value);
    CHECK(read_ns == 2);
    result.value();
    CHECK(read_ns == 3);

    result = channel.read(str, nullptr);
    CHECK(result.blocked());
    CHECK(str.empty());
    REQUIRE(result.value);
    CHECK(read_ns2 == 0);
    result.value();
    CHECK(read_ns2 == 1);

    str = "hello";
    result = channel.write(str, nullptr);
    CHECK(result.success());

    channel.close();

    str = "world";
    result = channel.write(str, nullptr);
    CHECK(result.closed());
    CHECK(str == "world");

    result = channel.read(str, nullptr);
    CHECK(result.success());
    CHECK(str == "hello");

    result = channel.read(str, nullptr);
    CHECK(result.closed());
}

TEST_CASE("buffered_channel 10") {
    ac::io::buffered_channel<std::string> channel(10);

    int i = 0;
    while (true) {
        auto str = std::to_string(i);
        auto status = channel.write(str, nullptr);
        if (status.blocked()) {
            break;
        }
        ++i;
    }
    CHECK(i == 10);

    i = 0;
    while (true) {
        std::string str;
        auto result = channel.read(str, nullptr);
        if (result.blocked()) {
            break;
        }
        CHECK(str == std::to_string(i));
        ++i;
    }
    CHECK(i == 10);

    int o = 0;

    for (i = 0; i < 25; ++i) {
        auto str = std::to_string(i);
        auto result = channel.write(str, nullptr);
        CHECK(result.success());

        if (i > 5) {
            result = channel.read(str, nullptr);
            CHECK(result.success());
            CHECK(str == std::to_string(o));
            ++o;
        }
    }

    channel.close();

    std::string str = "nope";
    auto result = channel.write(str, nullptr);

    for (i = 0; i <= 5; ++i) {
        result = channel.read(str, nullptr);
        CHECK(result.success());
        CHECK(str == std::to_string(o));
        ++o;
    }

    result = channel.read(str, nullptr);
    CHECK(result.closed());
}
