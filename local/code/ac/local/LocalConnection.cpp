// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "LocalConnection.hpp"
#include <ac/Session.hpp>
#include <ac/SessionHandler.hpp>

#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>

#include <deque>
#include <mutex>

namespace asio = boost::asio;

namespace ac::local {

namespace {

class LocalSession final : public Session, public astl::enable_shared_from {
public:
    using Session::Session;

    std::shared_ptr<LocalSession> m_counterpart = nullptr;

    std::deque<Frame> m_activeQueue;
    Frame popActive() {
        auto frame = std::move(m_activeQueue.front());
        m_activeQueue.pop_front();
        return frame;
    }

    std::mutex m_consMutex;
    std::deque<Frame> m_consQueue;
    bool m_pollingInFrames = true;

    virtual std::optional<Frame> getInFrame() override {
        if (!m_activeQueue.empty()) {
            return popActive();
        }

        {
            std::lock_guard lock(m_consMutex);
            if (m_consQueue.empty()) return {};
            m_activeQueue.swap(m_consQueue);
        }

        return popActive();
    }

    virtual bool acceptOutFrames() const override {
        // bTODO: maybe add some limit here?
        return true;
    }

    void postInFrameAvailableToHandler() {
        m_executor->post([this, self = shared_from_this()]() {
            if (m_handler) {
                m_handler->shOnAvailableSessionInFrames();
            }
        });
    }

    void remotePushFrame(Frame&& frame) {
        std::lock_guard lock(m_consMutex);
        m_consQueue.push_back(std::move(frame));
        if (m_consQueue.size() == 1 && m_pollingInFrames) {
            // a new frame has arrived, so we notify the handler
            postInFrameAvailableToHandler();
            m_pollingInFrames = false;
        }
    }

    virtual bool pushOutFrame(Frame&& frame) override {
        if (!m_counterpart) return false;
        m_counterpart->remotePushFrame(std::move(frame));
        return true;
    }

    virtual void pollInFramesAvailable() override {
        if (!m_activeQueue.empty()) {
            postInFrameAvailableToHandler();
        }
        else {
            std::lock_guard lock(m_consMutex);

            if (m_pollingInFrames) return; // already polling

            if (!m_consQueue.empty()) {
                postInFrameAvailableToHandler();
            }
            else {
                m_pollingInFrames = true;
            }
        }
    }

    virtual void pollOutFramesAccepted() override {
        // always ready to accept
        m_executor->post([this, self = shared_from_this()]() {
            if (m_handler) {
                m_handler->shOnSessionAcceptsOutFrames();
            }
        });
    }

    virtual void remoteClosed() {
        m_executor->post([this, self = shared_from_this()]() {
            if (m_handler) {
                m_handler->shSessionClosed();
            }
            resetHandler({});
            m_counterpart.reset();
        });
    }

    virtual void close() override {
        m_handler->shSessionClosed();
        resetHandler({});
        if (m_counterpart) {
            m_counterpart->remoteClosed();
            m_counterpart.reset();
        }
    }
};

class LocalExecutor final : public SessionExecutor {
public:
    using Strand = asio::strand<asio::io_context::executor_type>;
    virtual void post(std::function<void()> task) override;
};

} // namespace

} // namespace ac::local
