// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "SyncSession.hpp"
#include <ac/SessionHandler.hpp>
#include <astl/throw_stdex.hpp>

namespace ac::local {

namespace {
struct SynSessionExecutor final : public SessionExecutor {
    std::vector<std::function<void()>> m_tasks;
    virtual void post(std::function<void()> task) {
        m_tasks.push_back(std::move(task));
    }
};
}

SyncSession::SyncSession(SessionHandlerPtr handler)
    : Session(std::make_shared<SynSessionExecutor>())
{
    resetHandler(handler);
    m_handler->shOpened();
}

SyncSession::~SyncSession() {
    close();
}

void SyncSession::put(Frame&& frame) {
    if (m_inFrame) throw_ex{} << "SyncSession::put: in-frame already present";
    m_inFrame = std::move(frame);
    if (m_handler) {
        m_handler->shOnAvailableSessionInFrames();
        runTasks();
    }
}

std::optional<Frame> SyncSession::get() noexcept {
    std::optional<Frame> ret;
    std::swap(ret, m_outFrame);
    return std::move(ret);
}

bool SyncSession::valid() const noexcept {
    return !!m_handler;
}

void SyncSession::runTasks() {
    auto& tasks = static_cast<SynSessionExecutor*>(m_executor.get())->m_tasks;
    for (auto& task : tasks) {
        task();
    }
    tasks.clear();
}

bool SyncSession::hasInFrames() const {
    return m_inFrame.has_value();
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

void SyncSession::close() {
    runTasks();
    m_handler->shClosed();
    resetHandler({});
}

}
