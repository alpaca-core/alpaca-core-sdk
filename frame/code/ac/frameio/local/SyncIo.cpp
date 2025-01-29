// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "SyncIo.hpp"
#include "../FrameWithStatus.hpp"
#include "../SessionHandler.hpp"
#include "../IoExecutor.hpp"
#include "../IoCommon.hpp"
#include <vector>
#include <functional>
#include <mutex>

namespace ac::frameio {

namespace {

using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

struct PendingIo {
    StreamPtr stream;

    Frame* m_frame = nullptr;
    TimePoint m_deadline;
    IoCb m_cb;

    explicit operator bool() const noexcept { return !!m_frame; }

    Status syncIo(Frame& f) {
        assert(!m_frame);
        return stream->stream(f, nullptr);
    }

    void reset(Frame* f, astl::timeout timeout, IoCb&& c) {
        assert(!m_frame);
        if (timeout.infinite()) {
            m_deadline = TimePoint::max();
        }
        else {
            m_deadline = Clock::now() + timeout.duration;
        }
        m_frame = f;
        m_cb = std::move(c);
    }

    bool complete(Status status) {
        assert(m_frame);
        auto frame = std::exchange(m_frame, nullptr);
        auto cb = std::exchange(m_cb, nullptr);

        cb(*frame, status);
        return true;
    }

    bool tryComplete() {
        assert(m_frame);
        assert(m_cb);

        auto status = stream->stream(*m_frame, nullptr);

        if (status.complete()) {
            return complete(status);
        }
        if (std::chrono::steady_clock::now() >= m_deadline) {
            return complete(status.setTimeout());
        }

        return false;
    }
};

struct Executor {
    std::vector<std::function<void()>> m_executingTasks;

    PendingIo pendingInput, pendingOutput;

    std::mutex m_taskMutex;
    std::vector<std::function<void()>> m_tasks;

    Executor(ReadStreamPtr input, WriteStreamPtr output) {
        pendingInput.stream = std::move(input);
        pendingOutput.stream = std::move(output);
    }

    void pushTask(std::function<void()> task) {
        std::lock_guard lock(m_taskMutex);
        m_tasks.push_back(std::move(task));
    }

    // return number of tasks executed
    size_t executeTasks() {
        {
            std::lock_guard lock(m_taskMutex);
            m_executingTasks.swap(m_tasks);
        }
        for (auto& task : m_executingTasks) {
            task();
        }
        auto count = m_executingTasks.size();
        m_executingTasks.clear();
        return count;
    }

    void execute() {
        if (pendingInput) {
            pendingInput.tryComplete();
        }

        while (executeTasks());

        if (pendingOutput) {
            pendingOutput.tryComplete();
        }
    }
};

using ExecutorPtr = std::shared_ptr<Executor>;

class SyncIo {
    ExecutorPtr m_executor;
    PendingIo& m_pendingIo;

public:
    SyncIo(ExecutorPtr executor, PendingIo& pendingIo) : m_executor(executor), m_pendingIo(pendingIo) {}

    Status io(Frame& frame) {
        return m_pendingIo.syncIo(frame);
    }
    void io(Frame& frame, astl::timeout timeout, IoCb&& cb) {
        m_pendingIo.reset(&frame, timeout, std::move(cb));
    }
    void close() {
        m_pendingIo.stream->close();
    }
};

using SyncInput = InputCommon<SyncIo>;
using SyncOutput = OutputCommon<SyncIo>;

struct SyncExecutor final : public IoExecutor {
    SyncExecutor(const ExecutorPtr& tq) : m_taskQueue(tq) {}
    ExecutorPtr m_taskQueue;
    virtual void post(std::function<void()> task) {
        m_taskQueue->pushTask(std::move(task));
    }
};

} // namespace

std::function<void()> Session_connectSync(SessionHandlerPtr handler, StreamEndpoint ep) {
    auto tq = std::make_shared<Executor>(std::move(ep.readStream), std::move(ep.writeStream));
    SessionHandler::init(
        handler,
        std::make_unique<SyncInput>(tq, tq->pendingInput),
        std::make_unique<SyncOutput>(tq, tq->pendingOutput),
        std::make_unique<SyncExecutor>(tq)
    );
    tq->execute(); // connect
    return [tq]() { tq->execute(); };
}

} // namespace ac::frameio
