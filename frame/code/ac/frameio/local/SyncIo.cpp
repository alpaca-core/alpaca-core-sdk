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

struct SyncExecutor;
using SyncExecutorPtr = std::shared_ptr<SyncExecutor>;

struct SyncExecutor : public IoExecutor, public astl::enable_shared_from {
    struct PendingIo {
        StreamPtr stream;

        Frame* m_frame = nullptr;
        TimePoint m_deadline;
        IoCb m_cb;

        SyncExecutorPtr m_executor;
        std::vector<PendingIo*>& m_queue;

        PendingIo(StreamPtr s, SyncExecutorPtr executor, std::vector<PendingIo*>& queue)
            : stream(std::move(s))
            , m_executor(executor)
            , m_queue(queue)
        {}

        explicit operator bool() const noexcept { return !!m_frame; }

        io::status io(Frame& f) {
            assert(!m_frame);
            return stream->stream(f, nullptr);
        }

        void io(Frame& f, astl::timeout timeout, IoCb&& c) {
            assert(!m_frame);
            if (timeout.is_infinite()) {
                m_deadline = TimePoint::max();
            }
            else {
                m_deadline = Clock::now() + timeout.duration;
            }
            m_frame = &f;
            m_cb = std::move(c);

            m_queue.push_back(this);
        }

        void close() {
            stream->close();
        }

        void complete(io::status status) {
            assert(m_frame);
            auto frame = std::exchange(m_frame, nullptr);
            auto cb = std::exchange(m_cb, nullptr);

            cb(*frame, status);
        }

        std::optional<io::status> tryComplete() {
            assert(m_frame);
            assert(m_cb);

            auto status = stream->stream(*m_frame, nullptr);

            if (status.complete()) {
                return status;
            }
            if (std::chrono::steady_clock::now() >= m_deadline) {
                return status.set_timeout();
            }

            return {};
        }
    };

    using SyncInput = InputCommon<PendingIo>;
    using SyncOutput = OutputCommon<PendingIo>;

    std::vector<std::function<void()>> m_executingTasks;

    std::vector<PendingIo*> m_pendingInputs, m_pendingOutputs;

    std::mutex m_taskMutex;
    std::vector<std::function<void()>> m_tasks;

    void post(std::function<void()> task) override {
        std::lock_guard lock(m_taskMutex);
        m_tasks.push_back(std::move(task));
    }

    virtual InputPtr attachInput(ReadStreamPtr stream) override {
        return std::make_unique<SyncInput>(std::move(stream), shared_from(this), m_pendingInputs);
    }
    virtual OutputPtr attachOutput(WriteStreamPtr stream) override {
        return std::make_unique<SyncOutput>(std::move(stream), shared_from(this), m_pendingOutputs);
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
                post([pendingIo, status]() {
                    pendingIo->complete(*status);
                });
                return true;
            }
            return false;
        };

        astl::erase_all_if(m_pendingInputs, tryComplete);
        while (executeTasks());

        astl::erase_all_if(m_pendingOutputs, tryComplete);
        while (executeTasks());
    }
};

} // namespace

std::function<void()> Session_connectSync(SessionHandlerPtr handler, StreamEndpoint ep) {
    auto tq = std::make_shared<SyncExecutor>();
    SessionHandler::init(
        handler,
        tq->attachInput(std::move(ep.readStream)),
        tq->attachOutput(std::move(ep.writeStream)),
        tq
    );
    tq->execute(); // connect
    return [tq]() { tq->execute(); };
}

} // namespace ac::frameio
