// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "SyncIo.hpp"
#include "../FrameWithStatus.hpp"
#include "../SessionHandler.hpp"
#include "../IoExecutor.hpp"
#include "../IoCommon.hpp"
#include <astl/qalgorithm.hpp>
#include <vector>
#include <functional>
#include <mutex>

namespace ac::frameio {

namespace {

using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

struct PendingIo {
    StreamPtr stream;

    PendingIo(StreamPtr s) : stream(std::move(s)) {}

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

    void complete(Status status) {
        assert(m_frame);
        auto frame = std::exchange(m_frame, nullptr);
        auto cb = std::exchange(m_cb, nullptr);

        cb(*frame, status);
    }

    std::optional<Status> tryComplete() {
        assert(m_frame);
        assert(m_cb);

        auto status = stream->stream(*m_frame, nullptr);

        if (status.complete()) {
            return status;
        }
        if (std::chrono::steady_clock::now() >= m_deadline) {
            return status.setTimeout();
        }

        return {};
    }
};

struct Executor {
    std::vector<std::function<void()>> m_executingTasks;

    std::vector<PendingIo*> m_pendingInputs, m_pendingOutputs;

    std::mutex m_taskMutex;
    std::vector<std::function<void()>> m_tasks;

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
        auto tryComplete = [this](PendingIo* pendingIo) {
            if (auto status = pendingIo->tryComplete()) {
                pushTask([pendingIo, status]() {
                    pendingIo->complete(*status);
                });
                return true;
            }
            return false;
        };

        astl::erase_all_if(m_pendingInputs, tryComplete);

        while (executeTasks());

        astl::erase_all_if(m_pendingOutputs, tryComplete);
    }
};

using ExecutorPtr = std::shared_ptr<Executor>;

class SyncIo {
    PendingIo m_pendingIo;
    ExecutorPtr m_executor;
    std::vector<PendingIo*>& m_queue;

public:
    SyncIo(StreamPtr stream, ExecutorPtr executor, std::vector<PendingIo*>& queue)
        : m_pendingIo(std::move(stream))
        , m_executor(executor)
        , m_queue(queue)
    {}

    Status io(Frame& frame) {
        return m_pendingIo.syncIo(frame);
    }
    void io(Frame& frame, astl::timeout timeout, IoCb&& cb) {
        m_pendingIo.reset(&frame, timeout, std::move(cb));
        m_queue.push_back(&m_pendingIo);
    }
    void close() {
        m_pendingIo.stream->close();
    }
};

using SyncInput = InputCommon<SyncIo>;
using SyncOutput = OutputCommon<SyncIo>;

struct SyncExecutor final : public IoExecutor {
    SyncExecutor(const ExecutorPtr& tq) : m_tq(tq) {}
    ExecutorPtr m_tq;
    virtual void post(std::function<void()> task) {
        m_tq->pushTask(std::move(task));
    }
    virtual InputPtr attachInput(ReadStreamPtr stream) override {
        return std::make_unique<SyncInput>(std::move(stream), m_tq, m_tq->m_pendingInputs);
    }
    virtual OutputPtr attachOutput(WriteStreamPtr stream) override {
        return std::make_unique<SyncOutput>(std::move(stream), m_tq, m_tq->m_pendingOutputs);
    }
};

} // namespace

std::function<void()> Session_connectSync(SessionHandlerPtr handler, StreamEndpoint ep) {
    auto tq = std::make_shared<Executor>();
    SessionHandler::init(
        handler,
        std::make_unique<SyncInput>(std::move(ep.readStream), tq, tq->m_pendingInputs),
        std::make_unique<SyncOutput>(std::move(ep.writeStream), tq, tq->m_pendingOutputs),
        std::make_unique<SyncExecutor>(tq)
    );
    tq->execute(); // connect
    return [tq]() { tq->execute(); };
}

} // namespace ac::frameio
