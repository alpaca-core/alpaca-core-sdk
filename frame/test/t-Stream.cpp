// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/frameio/StreamPtr.hpp>
#include <ac/frameio/Stream.hpp>
#include <doctest/doctest.h>

using namespace ac::frameio;

class TestReadStream final : public ReadStream {
public:
    int i = 0;
    virtual Status read(ac::Frame& f, OnBlockedFunc onBlocked) override {
        Status ret;
        if (i & 1) {
            f.op = "odd";
            if (onBlocked) {
                onBlocked()();
            }
            ret.setWaiting();
        }
        else {
            f.op = "even";
            ret.setSuccess();
        }
        ++i;
        return ret;
    }
    virtual void close() override {}
};

class TestWriteStream final : public WriteStream {
public:
    int i = 0;
    virtual Status write(ac::Frame& f, OnBlockedFunc onBlocked) override {
        Status ret;
        if (i & 1) {
            CHECK(f.op == "odd");
            f.op.clear();
            ret.setSuccess();
        }
        else {
            CHECK(f.op == "even");
            if (onBlocked) {
                onBlocked()();
            }
            ret.setWaiting();
        }
        ++i;
        return ret;
    }
    virtual void close() override {}
};

TEST_CASE("StreamPtr") {
    auto p1 = std::make_unique<TestReadStream>();
    auto p2 = std::make_unique<TestWriteStream>();

    int i = 0;
    auto func = [&] {
        return [&] {
            ++i;
        };
    };

    ac::Frame frame;

    CHECK(p1->read(frame, func).success());
    CHECK(i == 0);
    CHECK(frame.op == "even");

    CHECK(p2->write(frame, func).blocked());
    CHECK(i == 1);
    CHECK(frame.op == "even");

    StreamPtr s1 = std::move(p1);
    StreamPtr s2 = std::move(p2);

    CHECK(s1->stream(frame, func).blocked());
    CHECK(i == 2);
    CHECK(frame.op == "odd");

    CHECK(s2->stream(frame, func).success());
    CHECK(i == 2);
    CHECK(frame.op.empty());
}
