// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/io/buffered_channel.hpp>
#include <ac/io/buffered_channel_endpoints.hpp>
#include <ac/io/sync_io.hpp>
#include <ac/io/xio_endpoint.hpp>

#include <ac/xec/timer_wobj.hpp>
#include <ac/xec/strand_wobj.hpp>
#include <ac/xec/coro.hpp>
#include <ac/xec/context.hpp>
#include <ac/xec/context_work_guard.hpp>
#include <ac/xec/thread_runner.hpp>
#include <ac/xec/co_spawn.hpp>

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

class broadcast_service : public astl::enable_shared_from {
public:
    ac::xec::strand ex;

    struct session;

    int broadcast_id = -1;

    struct broadcast_data {
        std::shared_ptr<session> s;
        int id;
    };
    std::deque<broadcast_data> broadcast_queue;

    struct session : public astl::enable_shared_from {
        std::shared_ptr<broadcast_service> service;
        xio_ep io;
        int last_responded_id;
        bool dead = false;

        session(stream_ep ep, const std::shared_ptr<broadcast_service>& service)
            : service(service)
            , io(std::move(ep), service->ex)
            , last_responded_id(service->broadcast_id)
        {}

        ac::xec::coro<void> run(std::shared_ptr<void> self) {
            try {
                service->sessions.push_back(shared_from(this));
                auto& queue = service->broadcast_queue;
                while (true) {
                    auto req = co_await io.poll();
                    if (req.value == "response") {
                        REQUIRE_FALSE(queue.empty());
                        auto& cur = queue.front();
                        REQUIRE(cur.id != -1);
                        last_responded_id = cur.id;
                        co_await service->check_broadcast_complete();
                    }
                    else {
                        REQUIRE(req.value == "initiate");
                        queue.push_back({shared_from(this), -1});
                        co_await service->broadcast();
                        last_responded_id = service->broadcast_id;
                    }
                }
            }
            catch (ac::io::stream_closed_error&) {
                dead = true;
            }
        }
    };

    std::vector<std::shared_ptr<session>> sessions;

    ac::xec::coro<void> broadcast() {
        REQUIRE_FALSE(broadcast_queue.empty());
        auto& b = broadcast_queue.front();
        if (b.id != -1) co_return;

        b.id = ++broadcast_id;

        for (auto& s : sessions) {
            if (!s) continue;
            if (s == b.s) continue;

            auto res = co_await s->io.push<false>("request");
            if (res.closed()) {
                s->dead = true;
                s = nullptr; // gc
            }
            else {
                REQUIRE(res.success());
            }
        }
    }

    ac::xec::coro<void> check_broadcast_complete() {
        REQUIRE_FALSE(broadcast_queue.empty());

        for (auto& s : sessions) {
            if (!s) continue;
            if (s->dead) {
                s = nullptr; // gc
                continue;
            }

            if (s->last_responded_id != broadcast_id) {
                co_return;
            }
        }

        co_await broadcast_queue.front().s->io.push<false>("done");

        broadcast_queue.pop_front();
        while (!broadcast_queue.empty() && broadcast_queue.front().s->dead) {
            broadcast_queue.pop_front();
        }

        if (!broadcast_queue.empty()) {
            co_await broadcast();
        }
    }

    void connect(stream_ep ep) {
        auto s = std::make_shared<session>(std::move(ep), shared_from(this));
        co_spawn(ex, s->run(s));
    }
};

struct worker_service_session {
    xio_ep broadcast_io;
    ac::xec::strand_wobj wobj;

    ac::xec::coro<void> handle_broadcast() {
        while (true) {
            auto req = co_await broadcast_io.poll<false>();
            if (req.closed()) co_return;
            REQUIRE(req.success());
            if (req.value == "request") {
                while (true) {
                    auto rsp = co_await broadcast_io.push<false>("response");
                    if (rsp.closed()) co_return;
                    if (rsp.success()) break;
                }
            }
            else {
                REQUIRE(req.value == "done");
                wobj.notify_one();
            }
        }
    }

    ac::xec::coro<void> run() {

    }
};

TEST_CASE("broadcast") {
    ac::xec::context xctx;
    auto wg = xctx.make_work_guard();
    ac::xec::thread_runner runner(xctx, 5, "test");

    wg.reset();
}