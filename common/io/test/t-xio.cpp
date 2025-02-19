// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/io/xio_endpoint.hpp>
#include <ac/io/buffered_channel.hpp>
#include <ac/io/channel_stream.hpp>
#include <ac/io/channel_endpoints.hpp>
#include <ac/io/blocking_io.hpp>

#include <ac/xec/timer_wobj.hpp>
#include <ac/xec/strand_wobj.hpp>
#include <ac/xec/coro.hpp>
#include <ac/xec/context.hpp>
#include <ac/xec/context_work_guard.hpp>

#include <doctest/doctest.h>

#include <astl/multi_thread_runner.hpp>
#include <astl/shared_from.hpp>

#include <optional>
#include <string>
#include <deque>


template <typename Frame>
struct t_io {
    using channel = ac::io::buffered_channel<Frame>;
    using read_stream = ac::io::channel_read_stream<channel>;
    using write_stream = ac::io::channel_write_stream<channel>;
    using stream_endpoint = ac::io::stream_endpoint<read_stream, write_stream>;

    template <typename Wobj>
    using xep = ac::io::xio_endpoint<read_stream, write_stream, Wobj>;

    using t_ep = xep<ac::xec::timer_wobj>;
    using s_ep = xep<ac::xec::strand_wobj>;

    static ac::io::channel_endpoints<channel, channel> make_channel_endpoints(size_t ab, size_t ba) {
        return ac::io::make_channel_endpoints(
            std::make_unique<channel>(ab),
            std::make_unique<channel>(ba)
        );
    }
};

using int_io = t_io<int>;
using string_io = t_io<std::string>;

struct echo_session_handler : public astl::enable_shared_from {
    string_io::t_ep m_io;

    echo_session_handler(ac::xec::strand s) : m_io(s) {}

    void run(string_io::stream_endpoint ep) {
        m_io.attach(std::move(ep));
        post(m_io.get_executor(), [this, pl = shared_from_this()]() mutable {
            read();
        });
    }

    std::string m_str;

    void read() {
        m_io.poll(m_str, [this, pl = shared_from_this()](std::string&, ac::io::status status) {
            if (status.closed()) return;
            write();
        });
    }

    void write() {
        m_str += " echo";
        m_io.push(m_str, [this, pl = shared_from_this()](std::string&, ac::io::status status) {
            if (status.closed()) return;
            read();
        });
    }
};

TEST_CASE("echo session handler") {
    ac::xec::context ctx;
    auto wg = ctx.make_work_guard();
    astl::multi_thread_runner runner(ctx, 2);

    auto [local, remote] = string_io::make_channel_endpoints(3, 3);

    {
        auto sh = std::make_shared<echo_session_handler>(ctx.make_strand());
        sh->run(std::move(remote));
    }

    ac::io::blocking_io io(std::move(local));

    io.push("hello");
    auto r = io.poll();
    CHECK(r.success());
    CHECK(r.value == "hello echo");

    io.push("asdf");

    const std::string long_str = "the quick brown fox jumps over the lazy dog";
    io.push(std::string(long_str));

    r = io.poll();
    CHECK(r.success());
    CHECK(r.value == "asdf echo");

    r = io.poll();
    CHECK(r.success());
    CHECK(r.value == long_str + " echo");

    wg.reset();
}

struct multi_echo_session_handler : public astl::enable_shared_from {
    string_io::t_ep m_io;

    multi_echo_session_handler(ac::xec::strand s) : m_io(s) {}

    void run(string_io::stream_endpoint ep) {
        m_io.attach(std::move(ep));
        post(m_io.get_executor(), [this, pl = shared_from_this()]() mutable {
            read();
        });
    }

    std::string m_in;
    std::deque<std::string> m_out_queue;
    std::optional<std::string> m_writing;

    void read() {
        m_io.poll(m_in, [this, pl = shared_from_this()](std::string&, ac::io::status status) {
            if (status.closed()) return;
            int n = std::stoi(m_in);
            m_in += " echo";
            for (int i = 0; i < n; ++i) {
                m_out_queue.push_back(m_in);
            }
            try_write();
            read();
        });
    }

    void try_write() {
        if (m_writing) return; // write in progress
        if (m_out_queue.empty()) return; // nothing to write

        m_writing = std::move(m_out_queue.front());
        m_out_queue.pop_front();

        m_io.push(*m_writing, [this, pl = shared_from_this()](std::string&, ac::io::status status) {
            if (status.closed()) return;
            m_writing.reset();
            try_write();
        });
    }
};


TEST_CASE("multi echo session handler") {
    ac::xec::context ctx;
    auto wg = ctx.make_work_guard();
    astl::multi_thread_runner runner(ctx, 2);

    auto [local, remote] = string_io::make_channel_endpoints(3, 3);

    {
        auto sh = std::make_shared<multi_echo_session_handler>(ctx.make_strand());
        sh->run(std::move(remote));
    }

    ac::io::blocking_io io(std::move(local));

    io.push("1 one");
    auto r = io.poll();
    CHECK(r.success());
    CHECK(r.value == "1 one echo");

    r = io.poll(astl::timeout::immediately());
    CHECK(r.timeout());

    io.push("2 and");
    io.push("3 and");
    io.push("4 and");
    io.push("5 and");

    for (int i = 2; i <= 5; ++i) {
        std::string exp = std::to_string(i) + " and echo";
        for (int j = 0; j < i; ++j) {
            r = io.poll();
            CHECK(r.success());
            CHECK(r.value == exp);
        }
    }

    r = io.poll(astl::timeout::immediately());
    CHECK(r.timeout());

    wg.reset();
}

ac::xec::coro<void> addition_service(int_io::stream_endpoint ep) {
    auto ex = co_await ac::xec::executor{};
    int_io::t_ep io(std::move(ep), ex);

    try {
        while (true) {
            auto a = co_await io.poll();
            auto b = co_await io.poll();
            co_await io.push(a.value + b.value);
        }
    }
    catch (ac::io::stream_closed_error&) {
        co_return;
    }
}

TEST_CASE("coro addition") {
    auto [local, remote] = int_io::make_channel_endpoints(6, 3);

    ac::xec::context ctx;
    co_spawn(ctx, addition_service(std::move(remote)));
    astl::multi_thread_runner runner(ctx, 2);

    ac::io::blocking_io io(std::move(local));

    io.push(1);
    io.push(2);
    auto r = io.poll();
    CHECK(r.success());
    CHECK(r.value == 3);

    io.push(10);
    io.push(20);

    io.push(42);
    io.push(800);

    r = io.poll();
    CHECK(r.success());
    CHECK(r.value == 30);

    r = io.poll();
    CHECK(r.success());
    CHECK(r.value == 842);
}


ac::xec::coro<void> string_addition_service(string_io::stream_endpoint user_ep, int_io::stream_endpoint service_ep) {
    auto ex = co_await ac::xec::executor{};
    string_io::t_ep user(std::move(user_ep), ex);
    int_io::t_ep service(std::move(service_ep), ex);

    while (true) {
        auto r = co_await user.poll();
        if (r.value == "exit") {
            co_return;
        }

        auto i = std::stoi(r.value);
        co_await service.push(i);
        co_await service.push(10);
        auto result = co_await service.poll();
        co_await user.push(std::to_string(result.value));
    }
}


TEST_CASE("coro multi") {
    auto [e_here, e_ss] = ac::io::make_channel_endpoints(
        std::make_unique<string_io::channel>(3),
        std::make_unique<string_io::channel>(3)
    );

    auto [e_si, e_is] = ac::io::make_channel_endpoints(
        std::make_unique<int_io::channel>(6),
        std::make_unique<int_io::channel>(3)
    );

    ac::io::blocking_io here(std::move(e_here));

    ac::xec::context ctx;

    co_spawn(ctx, addition_service(std::move(e_is)));
    co_spawn(ctx, string_addition_service(std::move(e_ss), std::move(e_si)));

    astl::multi_thread_runner runner(ctx, 3);

    here.push("1");
    here.push("12");

    auto r = here.poll();
    CHECK(r.value == "11");

    r = here.poll();
    CHECK(r.value == "22");

    here.push("exit");
}
