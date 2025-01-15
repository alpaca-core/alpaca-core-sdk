// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

// a single consumeer queue
// it is not lockless, but locks rarely which allows higher throughput
// if messages come in bursts
#pragma once
#include <deque>
#include <mutex>
#include <optional>

namespace astl {

template <typename T>
class sc_queue {
    std::deque<T> m_cons;

    std::mutex m_mutex;
    std::deque<T> m_prod;

    T pop_cons() {
        // assuming m_cons is not empty here
        T ret = std::move(m_cons.front());
        m_cons.pop_front();
        return ret;
    }

public:
    sc_queue() = default;
    sc_queue(const sc_queue&) = delete;
    sc_queue& operator=(const sc_queue&) = delete;

    template <typename U>
    void push(U&& t) {
        std::lock_guard lock(m_mutex);
        m_prod.push_back(std::forward<U>(t));
    }

    std::optional<T> try_pop() {
        if (!m_cons.empty()) {
            return pop_cons();
        }

        {
            std::lock_guard lock(m_mutex);
            if (m_prod.empty()) return {};
            std::swap(m_cons, m_prod);
        }

        return pop_cons();
    }
};

} // namespace astl
