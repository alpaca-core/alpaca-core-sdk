// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#pragma once
#include <thread>
#include <vector>
#include <cassert>

// run an asio-like context in multiple threads

namespace astl {
template <typename Ctx>
class multi_thread_runner {
    std::vector<std::thread> m_threads; // would use jthread, but apple clang still doesn't support them
public:
    multi_thread_runner() = default;

    void start(Ctx& ctx, size_t n) {
        assert(m_threads.empty());
        if (!m_threads.empty()) return; // rescue
        m_threads.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            m_threads.push_back(std::thread([&]() {
                ctx.run();
            }));
        }
    }

    void join() {
        for (auto& t : m_threads) {
            t.join();
        }
        m_threads.clear();
    }

    multi_thread_runner(Ctx& ctx, size_t n) {
        start(ctx, n);
    }

    ~multi_thread_runner() {
        join();
    }

    size_t num_threads() const noexcept {
        return m_threads.size();
    }

    bool empty() const noexcept {
        return m_threads.empty();
    }
};

}

