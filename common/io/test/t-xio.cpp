// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/io/xio_endpoint.hpp>
#include <ac/io/buffered_channel.hpp>
#include <ac/io/channel_stream.hpp>
#include <ac/io/buffered_channel_endpoints.hpp>
#include <ac/io/blocking_io.hpp>

#include <ac/xec/timer_wobj.hpp>
#include <ac/xec/strand_wobj.hpp>
#include <ac/xec/coro.hpp>
#include <ac/xec/context.hpp>
#include <ac/xec/context_work_guard.hpp>
#include <ac/xec/multi_thread_runner.hpp>
#include <ac/xec/co_spawn.hpp>

#include <doctest/doctest.h>

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

    static auto make_channel_endpoints(size_t ab, size_t ba) {
        return ac::io::make_buffered_channel_endpoints<Frame>(ab, ba);
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

void test_echo(ac::io::blocking_io<string_io::read_stream, string_io::write_stream>& io) {
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
}

TEST_CASE("echo session handler") {
    ac::xec::context xctx;
    auto wg = xctx.make_work_guard();
    ac::xec::multi_thread_runner runner(xctx, 2);

    auto [local, remote] = string_io::make_channel_endpoints(3, 3);

    {
        auto sh = std::make_shared<echo_session_handler>(xctx.make_strand());
        sh->run(std::move(remote));
    }

    ac::io::blocking_io io(std::move(local));

    test_echo(io);

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
    ac::xec::context xctx;
    auto wg = xctx.make_work_guard();
    ac::xec::multi_thread_runner runner(xctx, 2);

    auto [local, remote] = string_io::make_channel_endpoints(3, 3);

    {
        auto sh = std::make_shared<multi_echo_session_handler>(xctx.make_strand());
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

ac::xec::coro<int> do_addition(int_io::t_ep& io) {
    auto a = co_await io.poll();
    auto b = co_await io.poll();
    co_return a.value + b.value;
}

ac::xec::coro<void> addition_service(int_io::stream_endpoint ep) {
    auto ex = co_await ac::xec::executor{};
    int_io::t_ep io(std::move(ep), ex);

    try {
        while (true) {
            auto sum = co_await do_addition(io);
            co_await io.push(sum);
        }
    }
    catch (ac::io::stream_closed_error&) {
        co_return;
    }
}

TEST_CASE("coro addition") {
    auto [local, remote] = int_io::make_channel_endpoints(6, 3);

    ac::xec::context xctx;
    co_spawn(xctx, addition_service(std::move(remote)));
    ac::xec::multi_thread_runner runner(xctx, 2);

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

    ac::xec::multi_thread_runner runner(ctx, 3);

    here.push("1");
    here.push("12");

    auto r = here.poll();
    CHECK(r.value == "11");

    r = here.poll();
    CHECK(r.value == "22");

    here.push("exit");
}

int a_rec_sum = 0;
bool a_done = true;

ac::xec::coro<void> side_a(string_io::stream_endpoint ep, int send) {
    using namespace astl::timeout_vals;

    auto ex = co_await ac::xec::executor{};
    string_io::t_ep io(std::move(ep), ex);

    for (int i = 1; i <= send; ++i) {
        auto res = co_await io.poll(no_wait);
        if (res.success()) {
            a_rec_sum += std::stoi(res.value);
        }
        co_await io.push(std::to_string(i));
    }

    co_await io.push("end");

    try {
        while (true) {
            auto res = co_await io.poll();
            if (res.success()) {
                a_rec_sum += std::stoi(res.value);
            }
        }
    }
    catch (const ac::io::stream_closed_error&) {
        a_done = true;
    }
}

int b_rec_sum = 0;

ac::xec::coro<void> side_b(string_io::stream_endpoint ep, int send) {
    using namespace astl::timeout_vals;
    using namespace std::chrono_literals;

    bool receive = true;

    auto ex = co_await ac::xec::executor{};
    string_io::t_ep io(std::move(ep), ex);

    while (send || receive) {
        if (send) {
            co_await io.push(std::to_string(1000 + send));
            --send;
        }
        if (receive) {
            auto f = co_await io.poll(await_completion_for(20ms));
            if (f.success()) {
                if (f.value == "end") {
                    receive = false;
                }
                else {
                    b_rec_sum += std::stoi(f.value);
                }
            }
        }
    }
}

TEST_CASE("coro two-side") {
    auto [e_a, e_b] = ac::io::make_channel_endpoints(
        std::make_unique<string_io::channel>(5),
        std::make_unique<string_io::channel>(5)
    );

    ac::xec::context ctx;

    const int asend = 10, bsend = 15;
    co_spawn(ctx, side_a(std::move(e_a), asend));
    co_spawn(ctx, side_b(std::move(e_b), bsend));

    ac::xec::multi_thread_runner(ctx, 3);

    CHECK(a_rec_sum == 1000 * bsend + (bsend * (bsend + 1)) / 2);
    CHECK(a_done);
    CHECK(b_rec_sum == (asend * (asend + 1)) / 2);
}

ac::xec::coro<void> eager(string_io::stream_endpoint ep) {
    auto ex = co_await ac::xec::executor{};
    string_io::t_ep io(std::move(ep), ex);

    auto fin = io.get();
    CHECK(fin.success());
    auto i = std::stoi(fin.value);
    while (i) {
        std::string f = "hi";
        while (!io.put(f).success()); // spin
        --i;
    }
}

TEST_CASE("coro eager") {
    auto [local, remote] = string_io::make_channel_endpoints(3, 3);

    ac::xec::context ctx;
    co_spawn(ctx, eager(std::move(remote)));

    ac::io::blocking_io io(std::move(local));
    io.push("10");

    ac::xec::multi_thread_runner runner(ctx, 2);

    int received = 0;
    while (true) {
        auto f = io.poll();
        if (!f.success()) break;
        CHECK(f.value == "hi");
        ++received;
    }
    CHECK(received == 10);
}

ac::xec::coro<void> echo_prelude(string_io::stream_endpoint ep) {
    auto ex = co_await ac::xec::executor{};
    string_io::t_ep io(std::move(ep), ex);

    int i = 0;
    while (true) {
        auto fin = co_await io.poll();
        auto& op = fin.value;
        if (op == "goto echo") {
            std::make_shared<echo_session_handler>(ex)->run(io.detach());
            co_return;
        }
        else if (op == "i") {
            co_await io.push(std::to_string(i++));
        }
        else {
            co_return;
        }
    }
}

TEST_CASE("detach to successor") {
    ac::xec::context ctx;
    auto wg = ctx.make_work_guard();
    ac::xec::multi_thread_runner runner(ctx, 2);

    {
        auto [local, remote] = string_io::make_channel_endpoints(3, 3);
        ac::io::blocking_io io(std::move(local));
        co_spawn(ctx, echo_prelude(std::move(remote)));

        io.push("i");
        auto f = io.poll();
        CHECK(f.success());
        CHECK(f.value == "0");

        io.push("i");
        f = io.poll();
        CHECK(f.success());
        CHECK(f.value == "1");

        io.push("nope");
        f = io.poll();
        CHECK(f.closed());
    }

    {
        auto [local, remote] = string_io::make_channel_endpoints(3, 3);
        co_spawn(ctx, echo_prelude(std::move(remote)));
        ac::io::blocking_io io(std::move(local));

        io.push("i");
        auto f = io.poll();
        CHECK(f.success());
        CHECK(f.value == "0");

        io.push("i");
        f = io.poll();
        CHECK(f.success());
        CHECK(f.value == "1");

        io.push("goto echo");
        test_echo(io);
    }

    wg.reset();
}
