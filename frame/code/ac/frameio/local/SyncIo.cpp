// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "SyncIo.hpp"
#include "../FrameWithStatus.hpp"
#include "../IoCommon.hpp"
#include "../BasicStreamIo.hpp"
#include "../SessionHandler.hpp"
#include "../IoExecutor.hpp"
#include <vector>
#include <functional>
#include <mutex>

namespace ac::frameio {

namespace {

struct SyncTaskQueue {
    std::vector<std::function<void()>> m_executingTasks;

    std::mutex m_mutex;
    std::vector<std::function<void()>> m_tasks;

    void push(std::function<void()> task) {
        std::lock_guard lock(m_mutex);
        m_tasks.push_back(std::move(task));
    }

    void execute() {
        {
            std::lock_guard lock(m_mutex);
            m_executingTasks.swap(m_tasks);
        }
        for (auto& task : m_executingTasks) {
            task();
        }
        m_executingTasks.clear();
    }
};

using SyncTaskQueuePtr = std::shared_ptr<SyncTaskQueue>;

struct SyncIo : public BasicStreamIo {
    SyncTaskQueuePtr m_taskQueue;

    SyncIo(StreamPtr&& stream, const SyncTaskQueuePtr& tq)
        : BasicStreamIo(std::move(stream))
        , m_taskQueue(tq)
    {}

    FrameRefWithStatus io(Frame& frame) {
        auto status = m_stream->stream(frame, nullptr);
        return FrameRefWithStatus(frame, status);
    }

    void do_io(Frame& frame, std::chrono::steady_clock::time_point deadline, IoCb cb) {
        auto status = m_stream->stream(frame, nullptr);
        if (status.complete()) {
            cb(frame, status);
            return;
        }
        if (std::chrono::steady_clock::now() >= deadline) {
            cb(frame, status.setTimeout());
            return;
        }
        m_taskQueue->push([this, &frame, deadline, cb = std::move(cb)]() mutable {
            do_io(frame, deadline, std::move(cb));
        });
    }

    void io(Frame& frame, astl::timeout timeout, IoCb cb) {
        std::chrono::steady_clock::time_point deadline;
        if (timeout.infinite()) {
            deadline = std::chrono::steady_clock::time_point::max();
        }
        else {
            deadline = std::chrono::steady_clock::now() + timeout.duration;
        }

        do_io(frame, deadline, std::move(cb));
    }
};

using SyncInput = InputCommon<SyncIo>;
using SyncOutput = OutputCommon<SyncIo>;

struct SyncExecutor final : public IoExecutor {
    SyncExecutor(const SyncTaskQueuePtr& tq) : m_taskQueue(tq) {}
    SyncTaskQueuePtr m_taskQueue;
    virtual void post(std::function<void()> task) {
        m_taskQueue->push(std::move(task));
    }
};

} // namespace

std::function<void()> Session_connectSync(SessionHandlerPtr handler, StreamEndpoint ep) {
    auto tq = std::make_shared<SyncTaskQueue>();
    SessionHandler::init(
        *handler,
        std::make_unique<SyncInput>(std::move(ep.readStream), tq),
        std::make_unique<SyncOutput>(std::move(ep.writeStream), tq),
        std::make_unique<SyncExecutor>(tq)
    );
    return [tq]() { tq->execute(); };
}

} // namespace ac::frameio
