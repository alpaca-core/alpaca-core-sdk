// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/io/buffered_channel.hpp>
#include <ac/io/buffered_channel_endpoints.hpp>
#include <ac/io/sync_io.hpp>
#include <ac/io/xio_endpoint.hpp>

#include <ac/xec/timer_wobj.hpp>
#include <ac/xec/coro.hpp>
#include <ac/xec/context.hpp>
#include <ac/xec/context_work_guard.hpp>
#include <ac/xec/multi_thread_runner.hpp>

#include <doctest/doctest.h>

#include <astl/shared_from.hpp>

#include <string>
#include <vector>
#include <deque>

using channel = ac::io::buffered_channel<std::string>;
using read_stream = ac::io::channel_read_stream<channel>;
using write_stream = ac::io::channel_write_stream<channel>;
using stream_ep = ac::io::stream_endpoint<read_stream, write_stream>;
using xio_ep = ac::io::xio_endpoint<read_stream, write_stream, ac::xec::timer_wobj>;

struct broadcast_service_coro : public ac::xec::coro_state {
    xio_ep io;

    broadcast_service_coro(stream_ep ep, ac::xec::strand ex)
        : ac::xec::coro_state(ex)
        , io(std::move(ep), ex)
    {}

    struct shared_state {
        std::vector<std::shared_ptr<broadcast_service_coro>> sessions;
        std::deque<broadcast_service_coro*> pending;
    };
    std::shared_ptr<shared_state> m_shared_state;

    ac::xec::coro<void> broadcast() {
        REQUIRE(m_shared_state->pending.front() == this);
        for (auto& s : m_shared_state->sessions) {
            if (!s || s.get() == this) continue;
            try {
                co_await s->io.push("req");
            }
            catch (ac::io::stream_closed_error&) {
                s = nullptr;
            }
        }
    }

    ac::xec::coro<void> run(stream_ep sep) {
        while (true) {
            auto req = co_await io.poll();
            if (req.value == "req") {
                m_shared_state->pending.push_back(this);
                if (m_shared_state->pending.size() == 1) {
                    co_await broadcast();
                }
            }
        }
    }
};

TEST_CASE("broadcast") {
    ac::xec::context xctx;
    auto wg = xctx.make_work_guard();
    ac::xec::multi_thread_runner runner(xctx, 5, "test");

    wg.reset();
}