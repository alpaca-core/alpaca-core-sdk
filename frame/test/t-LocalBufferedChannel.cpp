// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/frameio/local/LocalBufferedChannel.hpp>
#include <ac/frameio/local/LocalChannelUtil.hpp>
#include <ac/frameio/StreamPtr.hpp>
#include <doctest/doctest.h>

using namespace ac::frameio;

TEST_CASE("LocalBufferedChannel 1") {
    auto [readStream, writeStream] = LocalChannel_getStreams(LocalBufferedChannel_create(1));

    ac::Frame frame;
    ac::Frame frame2;

    auto status = readStream->read(frame, nullptr);
    CHECK(status.blocked());

    frame.op = "test";
    status = writeStream->write(frame, nullptr);
    CHECK(status.success());

    frame2.op = "nope";
    status = writeStream->write(frame2, nullptr);
    CHECK(status.blocked());

    status = readStream->read(frame2, nullptr);
    CHECK(status.success());
    CHECK(frame2.op == "test");

    status = readStream->read(frame2, nullptr);
    CHECK(status.blocked());

    auto pl = std::make_shared<int>(42);

    int readUnblocked = 0;
    auto onReadBlocked = [&] {
        return [pl, &readUnblocked]() {
            ++readUnblocked;
        };
    };

    int writeUnblocked = 0;
    auto onWriteBlocked = [&] {
        return [pl, &writeUnblocked]() {
            ++writeUnblocked;
        };
    };

    status = readStream->read(frame2, onReadBlocked);
    CHECK(status.blocked());
    CHECK(status.waiting());
    CHECK(pl.use_count() == 2);
    CHECK(readUnblocked == 0);

    frame.op = "yep";
    status = writeStream->write(frame, onWriteBlocked);
    CHECK(status.success());
    CHECK(pl.use_count() == 1);
    CHECK(readUnblocked == 1);
    CHECK(writeUnblocked == 0);

    status = writeStream->write(frame2, onWriteBlocked);
    CHECK(status.blocked());
    CHECK(status.waiting());
    CHECK(pl.use_count() == 2);
    CHECK(readUnblocked == 1);
    CHECK(writeUnblocked == 0);

    status = readStream->read(frame2, onReadBlocked);
    CHECK(status.success());
    CHECK(frame2.op == "yep");
    CHECK(pl.use_count() == 1);
    CHECK(readUnblocked == 1);
    CHECK(writeUnblocked == 1);

    frame.op = "foo";
    status = writeStream->write(frame, nullptr);
    CHECK(status.success());

    readStream->close();

    status = writeStream->write(frame2, nullptr);
    CHECK(status.closed());

    status = readStream->read(frame2, nullptr);
    CHECK(status.success());
    CHECK(frame2.op == "foo");

    status = readStream->read(frame2, nullptr);
    CHECK(status.closed());
}

TEST_CASE("LocalBufferedChannel 10") {
    auto [readStream, writeStream] = LocalChannel_getStreams(LocalBufferedChannel_create(10));

    ac::Frame frame;
    int i = 0;
    while (true) {
        frame.op = std::to_string(i);
        auto status = writeStream->write(frame, nullptr);
        if (status.blocked()) {
            break;
        }
        ++i;
    }
    CHECK(i == 10);

    i = 0;
    while (true) {
        auto status = readStream->read(frame, nullptr);
        if (status.blocked()) {
            break;
        }
        CHECK(frame.op == std::to_string(i));
        ++i;
    }
    CHECK(i == 10);

    int o = 0;

    for (i = 0; i < 25; ++i) {
        frame.op = std::to_string(i);
        auto status = writeStream->write(frame, nullptr);
        CHECK(status.success());

        if (i > 5) {
            status = readStream->read(frame, nullptr);
            CHECK(status.success());
            CHECK(frame.op == std::to_string(o));
            ++o;
        }
    }

    writeStream->close();
    auto status = writeStream->write(frame, nullptr);
    CHECK(status.closed());

    for (i = 0; i <= 5; ++i) {
        status = readStream->read(frame, nullptr);
        CHECK(status.success());
        CHECK(frame.op == std::to_string(o));
        ++o;
    }

    status = readStream->read(frame, nullptr);
    CHECK(status.closed());
}

TEST_CASE("local endpoints") {
    auto [ab, ba] = LocalChannel_getEndpoints(
        LocalBufferedChannel_create(3),
        LocalBufferedChannel_create(5)
    );

    ac::Frame f0, f1;

    f0.op = "abc";
    auto status = ab.writeStream->write(f0, nullptr);
    CHECK(status.success());

    status = ab.readStream->read(f1, nullptr);
    CHECK(status.blocked());

    status = ba.readStream->read(f1, nullptr);
    CHECK(status.success());
    CHECK(f1.op == "abc");

    for (int i = 0; i < 3; ++i) {
        f0.op = std::to_string(i);
        status = ba.writeStream->write(f0, nullptr);
        CHECK(status.success());
    }
    status = ba.writeStream->write(f0, nullptr);
    CHECK(status.blocked());

    f0.op = "foo";
    status = ab.writeStream->write(f0, nullptr);
    CHECK(status.success());

    for (int i = 0; i < 3; ++i) {
        status = ab.readStream->read(f1, nullptr);
        CHECK(status.success());
        CHECK(f1.op == std::to_string(i));
    }
}
