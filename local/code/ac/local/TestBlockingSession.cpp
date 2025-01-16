// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "TestBlockingSession.hpp"
#include <ac/Session.hpp>
#include <ac/SessionHandler.hpp>
#include <deque>
#include <mutex>

namespace ac::local {

namespace {

struct alignas(std::hardware_destructive_interference_size) Queue {
    std::mutex mutex;
    std::condition_variable cv;
    std::deque<Frame> queue;
    bool polling;

    struct Lock : public std::unique_lock<std::mutex> {
        Queue& q;

        Lock(Queue& q) : std::unique_lock<std::mutex>(q.mutex), q(q) {}

        Queue& operator*() {
            return q;
        }
        Queue* operator->() {
            return &q;
        }

        std::optional<Frame> try_pop() {
            if (q.queue.empty()) return {};
            auto frame = std::move(q.queue.front());
            q.queue.pop_front();
            return frame;
        }
    };

    Lock lock() {
        return Lock(*this);
    }
};

class RemoteSession final : public Session, public astl::enable_shared_from {
public:
    using Session::Session;

    Queue m_in, m_out;

    virtual std::optional<Frame> getInFrame() override {
        auto l = m_in.lock();
        return l.try_pop();
    }

    virtual bool acceptOutFrames() const override {
        return true;
    }

    virtual bool pushOutFrame(Frame&& frame) override {
        bool notify = false;
        {
            auto l = m_out.lock();
            l->queue.push_back(std::move(frame));
            if (l->queue.size() == 1 && l->polling) {
                notify = true;
                l->polling = false;
            }
        }
        if (notify) {
            m_out.cv.notify_all();
        }
        return true;
    }

    void postInFrameAvailableToHandler() {
        m_executor->post([this, self = shared_from_this()]() {
            if (m_handler) {
                m_handler->shOnAvailableSessionInFrames();
            }
        });
    }

    virtual void pollInFramesAvailable() {
        auto l = m_in.lock();
        if (l->polling) return; // already polling

        if (!l->queue.empty()) {
            postInFrameAvailableToHandler();
        }
        else {
            l->polling = true;
        }
    }

    virtual void pollOutFramesAccepted() {
        m_executor->post([this, self = shared_from_this()]() {
            if (m_handler) {
                m_handler->shOnSessionAcceptsOutFrames();
            }
        });
    }

    void remoteAddInFrame(Frame&& frame) {
        auto l = m_in.lock();
        l->queue.push_back(std::move(frame));
        if (l->queue.size() == 1 && l->polling) {
            postInFrameAvailableToHandler();
            l->polling = false;
        }
    }

    std::optional<Frame> remoteAwaitOutFrame(int32_t mstimeout) {
        auto l = m_out.lock();
        auto ret = l.try_pop();
        if (ret || mstimeout == 0) return ret;
        if (mstimeout < 0) mstimeout = std::numeric_limits<int32_t>::max();
        l->cv.wait_for(l, std::chrono::milliseconds(mstimeout), [&] { return !l->queue.empty(); });
        return l.try_pop();
    }

    void close() override {
        m_handler->shSessionClosed();
        resetHandler({});
        m_out.cv.notify_all();
    }
};

}

class TestBlockingSession::Impl {
    std::shared_ptr<RemoteSession> m_session;

    std::optional<Frame> awaitInFrame(int32_t mstimeout) {
        if (!m_session) return {};
        return m_session->remoteAwaitOutFrame(mstimeout);
    }

    void pushOutFrame(Frame&& frame) {
        if (!m_session) return;
        m_session->remoteAddInFrame(std::move(frame));
    }
};

} // namespace ac::local
