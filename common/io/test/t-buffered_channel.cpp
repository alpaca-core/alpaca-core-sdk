// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/io/buffered_channel.hpp>
#include <ac/io/concepts/channel.hpp>
#include <doctest/doctest.h>
#include <string>

struct test_nobj final : public ac::xec::notifiable {
    int all = 0;
    int one = 0;
    void notify_all() override { ++all; }
    void notify_one() override { ++one; }
};

static_assert(ac::io::channel_class<ac::io::buffered_channel<std::string>>);

TEST_CASE("buffered_channel 1") {
    test_nobj read_nobj, read_nobj2, write_nobj;

    ac::io::buffered_channel<std::string> channel(1);

    auto* nobj = channel.exchange_read_nobj(nullptr);
    CHECK(nobj == nullptr);

    nobj = channel.exchange_write_nobj(nullptr);
    CHECK(nobj == nullptr);

    std::string str;
    auto result = channel.read(str, nullptr);
    CHECK(result.blocked());
    CHECK(result.value == nullptr);
    CHECK_FALSE(result.aborted());
    CHECK_FALSE(result.waiting());
    CHECK(str.empty());

    result = channel.read(str, &read_nobj);
    CHECK(result.value == nullptr);
    CHECK(result.blocked());
    CHECK(str.empty());
    CHECK(read_nobj.one == 0);

    nobj = channel.exchange_write_nobj(nullptr);
    CHECK(nobj == nullptr);

    nobj = channel.exchange_read_nobj(nullptr);
    CHECK(nobj == &read_nobj);

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
    result = channel.read(str, &read_nobj);
    CHECK(result.value == nullptr);
    CHECK(result.blocked());
    CHECK(result.waiting());
    CHECK(str.empty());

    str = "yep";
    result = channel.write(str, &write_nobj);
    CHECK(result.value == nullptr);
    CHECK(result.success());
    CHECK(read_nobj.one == 1);
    CHECK(str.empty());

    str = "nope again";
    result = channel.write(str, &write_nobj);
    CHECK(result.value == nullptr);
    CHECK(result.blocked());
    CHECK(result.waiting());
    CHECK(str == "nope again");

    result = channel.read(str, &read_nobj);
    CHECK(result.value == nullptr);
    CHECK(result.success());
    CHECK(str == "yep");
    CHECK(read_nobj.one == 1);

    str.clear();
    result = channel.read(str, &read_nobj);
    CHECK(result.value == nullptr);
    CHECK(result.blocked());
    CHECK(result.waiting());
    CHECK(str.empty());

    result = channel.read(str, &read_nobj2);
    CHECK(result.value == &read_nobj);
    CHECK(result.blocked());
    CHECK(result.waiting());
    CHECK(result.aborted());
    CHECK(str.empty());

    result = channel.read(str, nullptr);
    CHECK(result.value == &read_nobj2);
    CHECK(result.blocked());
    CHECK_FALSE(result.waiting());
    CHECK(result.aborted());
    CHECK(str.empty());

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

    CHECK(read_nobj.all == 0);
    CHECK(read_nobj.one == 1);
    CHECK(read_nobj2.all == 0);
    CHECK(read_nobj2.one == 0);
    CHECK(write_nobj.all == 0);
    CHECK(write_nobj.one == 1);
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
