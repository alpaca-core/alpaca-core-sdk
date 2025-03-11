// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "thread_name.hpp"
#include <thread>
#include <vector>
#include <cassert>

// run an asio-like context in multiple threads

namespace ac::xec {
class multi_thread_runner {
    std::vector<std::thread> m_threads; // would use jthread, but apple clang still doesn't support them
public:
    multi_thread_runner() = default;

    template <typename Ctx>
    void start(Ctx& ctx, size_t n, std::string_view name = {}) {
        assert(m_threads.empty());
        if (!m_threads.empty()) return; // rescue
        m_threads.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            m_threads.push_back(std::thread([i, n, &ctx, name = std::string(name)]() mutable {
                if (!name.empty()) {
                    // set thread name if not empty
                    if (n == 1) {
                        set_this_thread_name(name);
                    }
                    else {
                        name += ':';
                        // maybe pad with 0 depending on log10(n) some day
                        name += std::to_string(i);
                        set_this_thread_name(name);
                    }
                }
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

    template <typename Ctx>
    multi_thread_runner(Ctx& ctx, size_t n, std::string_view name = {}) {
        start(ctx, n, name);
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

} // namespace ac::xec
