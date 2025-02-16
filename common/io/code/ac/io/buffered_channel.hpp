// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "stream_result.hpp"
#include <ac/xec/notifiable.hpp>
#include <deque>
#include <mutex>
#include <cassert>

namespace ac::io {

template <typename T>
class buffered_channel {
public:
    using value_type = T;
    using read_value_type = T;
    using write_value_type = T;

    buffered_channel(size_t max_size)
        : m_max_size(max_size)
    {}

    xec::notifiable* exchange_read_nobj(xec::notifiable* other) {
        std::lock_guard lock(m_mutex);
        if (m_queue.empty()) {
            return exchange_nobj_l(other);
        }
        else {
            return nullptr;
        }
    }

    xec::notifiable* exchange_write_nobj(xec::notifiable* other) {
        std::lock_guard lock(m_mutex);
        if (m_queue.size() == m_max_size) {
            return exchange_nobj_l(other);
        }
        else {
            return nullptr;
        }
    }

    stream_result write(T& t, xec::notifiable* nobj) {
        std::unique_lock lock(m_mutex);
        if (m_closed) {
            assert(!m_nobj);
            return stream_result::build().set_closed();
        }
        else if (m_queue.size() < m_max_size) {
            assert(m_queue.empty() || !m_nobj);
            m_queue.push_back(std::move(t));
            unlock_notify(lock);
            return stream_result::build().set_success();
        }
        else {
            return block(nobj);
        }
    }

    stream_result read(T& t, xec::notifiable* nobj) {
        std::unique_lock lock(m_mutex);
        if (!m_queue.empty()) {
            assert(m_queue.size() == m_max_size || !m_nobj);
            t = std::move(m_queue.front());
            m_queue.pop_front();
            unlock_notify(lock);
            return stream_result::build().set_success();
        }
        else if (m_closed) {
            assert(!m_nobj);
            return stream_result::build().set_closed();
        }
        else {
            return block(nobj);
        }
    }

    void close() {
        std::unique_lock lock(m_mutex);
        m_closed = true;
        unlock_notify(lock);
    }
private:
    xec::notifiable* exchange_nobj_l(xec::notifiable* other) {
        return std::exchange(m_nobj, other);
    }

    stream_result block(xec::notifiable* nobj) {
        stream_result ret(exchange_nobj_l(nobj));
        if (ret.value) {
            ret.set_aborted();
        }
        if (m_nobj) {
            ret.set_waiting();
        }
        return ret;
    }

    void unlock_notify(std::unique_lock<std::mutex>& lock) {
        assert(lock.owns_lock());
        auto nobj = exchange_nobj_l(nullptr);
        lock.unlock();

        if (nobj) {
            nobj->notify_one();
        }
    }

    const size_t m_max_size;
    std::mutex m_mutex;
    std::deque<T> m_queue; // bTODO: ring buffer
    xec::notifiable* m_nobj = nullptr;
    bool m_closed = false;
};

} // namespace ac::frameio
