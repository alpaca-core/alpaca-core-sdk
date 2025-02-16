// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/frameio/local/BufferedChannel.hpp>
#include <ac/frameio/local/BufferedChannelStream.hpp>
#include <ac/frameio/local/BlockingIo.hpp>
#include <ac/frameio/local/BlockingSyncIoWrapper.hpp>
#include <ac/frameio/SessionHandler.hpp>
#include <ac/frameio/Io.hpp>
#include <doctest/doctest.h>
#include <deque>

using namespace ac;
using namespace ac::frameio;
using namespace astl::timeout_vals;

class MultiEcho final : public SessionHandler {
public:
    ac::Frame m_inFrame, m_outFrame;

    struct Echo {
        ac::Frame frame;
        int count;
    };
    std::deque<Echo> m_pending;
    std::optional<Echo> m_cur;

    virtual void shConnected() noexcept override {
        poll();
    }

    void poll() {
        shInput().poll(m_inFrame, await_completion, [this, pl = shared_from_this()](ac::Frame&, io::status status) {
            if (status.closed()) {
                // done
                return;
            }
            if (status.success()) {
                onRead();
            }
            poll();
        });
    }

    void onRead() {
        if (m_inFrame.op != "echo") return;
        auto& newEcho = m_pending.emplace_back();
        newEcho.frame.op = m_inFrame.data["msg"].get<std::string>();
        newEcho.count = m_inFrame.data["count"].get<int>();
        write();
    }

    void write() {
        if (m_cur) return; // already writing
        if (m_pending.empty()) return;
        m_cur = m_pending.front();
        m_pending.pop_front();

        writeCur();
    }

    void writeCur() {
        m_outFrame = m_cur->frame;
        shOutput().push(m_outFrame, await_completion, [this, pl = shared_from_this()](ac::Frame&, io::status status) {
            if (!status.complete()) {
                // retry
                writeCur();
                return;
            }
            if (status.closed()) {
                // done
                return;
            }
            onWriteCur();
        });
    }

    void onWriteCur() {
        --m_cur->count;
        if (m_cur->count > 0) {
            writeCur();
        }
        else {
            m_cur.reset();
            write();
        }
    }
};

TEST_CASE("SyncIo Wrapper") {
    BlockingSyncIoWrapper io(std::make_shared<MultiEcho>());
    CHECK(io.push(ac::Frame{"echo", {{"msg", "hello"}, {"count", 5}}}).success());
    for (int i = 0; i < 5; ++i) {
        auto f = io.poll();
        CHECK(f.success());
        CHECK(f.value.op == "hello");
    }
    CHECK(io.push(ac::Frame{"echo", {{"msg", "bye"}, {"count", 2}}}).success());
    for (int i = 0; i < 2; ++i) {
        auto f = io.poll();
        CHECK(f.success());
        CHECK(f.value.op == "bye");
    }
    auto f = io.poll();
    CHECK(f.blocked());
    io.close();
    f = io.poll();
    CHECK(f.closed());
}
