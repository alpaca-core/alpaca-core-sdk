// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalBufferedChannel.hpp"
#include "LocalChannel.hpp"
#include <deque>
#include <mutex>

namespace ac::frameio {

namespace {
class LocalBufferedChannel final : public LocalChannel {
    const size_t m_maxSize;
    std::mutex m_mutex;
    std::deque<Frame> m_queue; // bTODO: ring buffer
    Stream::NotifyCb m_notify;
    bool m_closed = false;

    Status block(Stream::OnBlockedFunc& onBlocked) {
        Status ret;
        if (m_notify) {
            ret.setAborted();
        }

        if (onBlocked) {
            m_notify = onBlocked();
            assert(m_notify); // no point in returning a null notify func, just use a null onBlocked
        }
        else {
            m_notify = {};
        }

        if (m_notify) {
            ret.setWaiting();
        }
        return ret;
    }

public:
    LocalBufferedChannel(size_t maxSize)
        : m_maxSize(maxSize)
    {}

    Status write(Frame& frame, Stream::OnBlockedFunc onBlocked) override {
        std::unique_lock lock(m_mutex);
        if (m_closed) {
            assert(!m_notify);
            return Status{}.setClosed();;
        }
        else if (m_queue.size() < m_maxSize) {
            assert(m_queue.empty() || !m_notify);
            std::function<void()> onPushFirst;
            std::swap(onPushFirst, m_notify);
            m_queue.push_back(std::move(frame));
            lock.unlock();
            if (onPushFirst) {
                onPushFirst();
            }
            return Status{}.setSuccess();
        }
        else {
            return block(onBlocked);
        }
    }

    Status read(Frame& frame, Stream::OnBlockedFunc onBlocked) override {
        std::unique_lock lock(m_mutex);
        if (!m_queue.empty()) {
            assert(m_queue.size() == m_maxSize || !m_notify);
            std::function<void()> onFree;
            std::swap(onFree, m_notify);
            frame = std::move(m_queue.front());
            m_queue.pop_front();
            lock.unlock();
            if (onFree) {
                onFree();
            }
            return Status{}.setSuccess();
        }
        else if (m_closed) {
            assert(!m_notify);
            return Status{}.setClosed();;
        }
        else {
            return block(onBlocked);
        }
    }

    void close() override {
        std::function<void()> notify;
        std::unique_lock lock(m_mutex);
        m_closed = true;
        std::swap(notify, m_notify);
        lock.unlock();

        if (notify) {
            notify();
        }
    }

    bool closed() override {
        std::lock_guard lock(m_mutex);
        return m_closed;
    }
};

} // namespace

LocalChannelPtr LocalBufferedChannel_create(size_t size) {
    return std::make_shared<LocalBufferedChannel>(size);
}

} // namespace ac::frameio
