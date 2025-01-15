// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "SyncSession.hpp"
#include <ac/SessionHandler.hpp>
#include <astl/throw_stdex.hpp>

namespace ac::local {

namespace {
struct SyncSessionExecutor final : public SessionExecutor {
    std::vector<std::function<void()>> m_tasks;
    virtual void post(std::function<void()> task) {
        m_tasks.push_back(std::move(task));
    }
};
}

SyncSession::SyncSession(SessionHandlerPtr handler)
    : Session(std::make_shared<SyncSessionExecutor>())
{
    resetHandler(handler);
}

SyncSession::~SyncSession() {
    close();
}

void SyncSession::put(Frame&& frame) {
    if (m_inFrame) throw_ex{} << "SyncSession::put: in-frame already present";
    m_inFrame = std::move(frame);
    if (m_pollInFrames) {
        m_pollInFrames = false;
        m_handler->shOnAvailableSessionInFrames();
    }
    runTasks();
}

std::optional<Frame> SyncSession::get() noexcept {
    runTasks();

    if (!m_outFrame) return {};

    std::optional<Frame> ret;
    std::swap(ret, m_outFrame);
    if (m_pollOutFrames) {
        m_pollOutFrames = false;
        m_handler->shOnSessionAcceptsOutFrames();
    }
    return ret;
}

bool SyncSession::valid() const noexcept {
    return !!m_handler;
}

void SyncSession::runTasks() {
    auto& tasks = static_cast<SyncSessionExecutor*>(m_executor.get())->m_tasks;
    for (auto& task : tasks) {
        task();
    }
    tasks.clear();
}

std::optional<Frame> SyncSession::getInFrame() {
    std::optional<Frame> ret;
    std::swap(ret, m_inFrame);
    return std::move(ret);
}

bool SyncSession::acceptOutFrames() const {
    return !m_outFrame.has_value();
}

bool SyncSession::pushOutFrame(Frame&& frame) {
    if (m_outFrame) return false;
    m_outFrame = std::move(frame);
    return true;
}

void SyncSession::pollInFramesAvailable() {
    m_pollInFrames = true;
}

void SyncSession::pollOutFramesAccepted() {
    m_pollOutFrames = true;
}

void SyncSession::close() {
    runTasks();
    m_handler->shSessionClosed();
    resetHandler({});
}

}
