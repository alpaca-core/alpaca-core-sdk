// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/io/buffered_channel.hpp>
#include <ac/io/buffered_channel_endpoints.hpp>
#include <ac/io/sync_io.hpp>
#include <ac/io/xio.hpp>

#include <ac/xec/timer_wobj.hpp>

#include <doctest/doctest.h>

#include <astl/shared_from.hpp>
#include <string>
#include <optional>

struct frame {
    std::string op;
    std::string msg;
    int count;
};

using channel = ac::io::buffered_channel<frame>;
using read_stream = ac::io::channel_read_stream<channel>;
using write_stream = ac::io::channel_write_stream<channel>;
using input = ac::io::xinput<read_stream, ac::xec::timer_wobj>;
using output = ac::io::xoutput<write_stream, ac::xec::timer_wobj>;

using namespace astl::timeout_vals;

class multi_echo final : public astl::enable_shared_from {
public:
    frame m_in_frame, m_out_frame;

    struct echo {
        frame f;
        int count;
    };
    std::deque<echo> m_pending;
    std::optional<echo> m_cur;

    input m_input;
    output m_output;

    multi_echo(ac::io::stream_endpoint<read_stream, write_stream> ep, ac::xec::strand strand)
        : m_input(std::move(ep.read_stream), strand)
        , m_output(std::move(ep.write_stream), strand)
    {}

    void connected() {
        poll();
    }

    void poll() {
        m_input.poll(m_in_frame, await_completion, [this, pl = shared_from_this()](frame&, ac::io::status status) {
            if (status.closed()) {
                // done
                return;
            }
            if (status.success()) {
                on_read();
            }
            poll();
        });
    }

    void on_read() {
        if (m_in_frame.op != "echo") return;
        auto& new_echo = m_pending.emplace_back();
        new_echo.f.op = m_in_frame.msg;
        new_echo.count = m_in_frame.count;
        write();
    }

    void write() {
        if (m_cur) return; // already writing
        if (m_pending.empty()) return;
        m_cur = m_pending.front();
        m_pending.pop_front();

        write_cur();
    }

    void write_cur() {
        m_out_frame = m_cur->f;
        m_output.push(m_out_frame, await_completion, [this, pl = shared_from_this()](frame&, ac::io::status status) {
            if (!status.complete()) {
                // retry
                write_cur();
                return;
            }
            if (status.closed()) {
                // done
                return;
            }
            on_write_cur();
        });
    }

    void on_write_cur() {
        --m_cur->count;
        if (m_cur->count > 0) {
            write_cur();
        }
        else {
            m_cur.reset();
            write();
        }
    }
};

TEST_CASE("sync io") {
    auto [elocal, eremote] = ac::io::make_buffered_channel_endpoints<frame>(3, 3);

    ac::io::sync_io_ctx ctx;
    ac::io::sync_io local(std::move(elocal), ctx);

    auto strand = ctx.make_strand();
    auto session_handler = std::make_shared<multi_echo>(std::move(eremote), strand);
    post(strand, [session_handler] {
        session_handler->connected();
    });

    local.put(frame{"echo", "hello", 5});
    local.put(frame{"echo", "bye", 2});

    for (int i = 0; i < 5; ++i) {
        auto f = local.get();
        CHECK(f.success());
        CHECK(f.value.op == "hello");
    }
    for (int i = 0; i < 2; ++i) {
        auto f = local.get();
        CHECK(f.success());
        CHECK(f.value.op == "bye");
    }
    auto f = local.get();
    CHECK(f.blocked());

    local.close();
}
