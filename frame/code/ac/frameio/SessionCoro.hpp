// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../export.h"
#include "SessionHandler.hpp"
#include "FrameWithStatus.hpp"
#include "IoException.hpp"
#include "StreamEndpoint.hpp"
#include <astl/expected.hpp>
#include <astl/timeout.hpp>
#include <coroutine>
#include <stdexcept>

namespace ac::frameio {

template <typename T>
struct SessionCoro;

namespace coro {
template <typename T>
using Result = astl::expected<T, std::exception_ptr>;
} // namespace coro

class CoroSessionHandler;
using CoroSessionHandlerPtr = std::shared_ptr<CoroSessionHandler>;

class AC_FRAME_EXPORT CoroSessionHandler final : public SessionHandler {
public:
    CoroSessionHandler() noexcept = default;
    ~CoroSessionHandler();

    static SessionHandlerPtr create(SessionCoro<void> coro);
    static SessionHandlerPtr create(SessionCoro<SessionHandlerPtr> coro);

    void postResume() noexcept;
    io::status getFrame(Input& in, Frame& frame) noexcept;
    void pollFrame(Input& in, Frame& frame, io::status& status, astl::timeout timeout) noexcept;
    io::status putFrame(Output& out, Frame& frame) noexcept;
    void pushFrame(Output& out, Frame& frame, io::status& status, astl::timeout timeout) noexcept;
    void close() noexcept;
private:
    template <typename T>
    friend struct SessionCoro;

    std::coroutine_handle<> m_currentCoro;

    coro::Result<SessionHandlerPtr> m_sucessorResult;

    std::coroutine_handle<> setCoro(std::coroutine_handle<> coro) noexcept {
        return std::exchange(m_currentCoro, coro);
    }

    virtual void shConnected() noexcept override;

    template <typename PromiseType>
    static CoroSessionHandlerPtr doCreate(std::coroutine_handle<PromiseType> h);
};

namespace coro {

namespace impl {
template <typename Io>
struct BasicFrameAwaitable {
    CoroSessionHandlerPtr handler;
    Io& io;
    Frame* frame;
    astl::timeout timeout;
    io::status status;

    BasicFrameAwaitable(const CoroSessionHandlerPtr& h, Io& io, Frame& f, astl::timeout t) noexcept
        : handler(h), io(io), frame(&f), timeout(t)
    {}
    bool await_ready() const noexcept { return false; }
};

struct BasicPollAwaitable : public BasicFrameAwaitable<Input> {
    using BasicFrameAwaitable<Input>::BasicFrameAwaitable;
    void await_suspend(std::coroutine_handle<>) noexcept {
        this->handler->pollFrame(this->io, *this->frame, this->status, this->timeout);
    }
};

struct BasicPushAwaitable : public BasicFrameAwaitable<Output> {
    using BasicFrameAwaitable<Output>::BasicFrameAwaitable;
    void await_suspend(std::coroutine_handle<>) noexcept {
        this->handler->pushFrame(this->io, *this->frame, this->status, this->timeout);
    }
};
} // namespace impl

template <bool E = true>
struct Poll: public impl::BasicPollAwaitable {
    Poll(const CoroSessionHandlerPtr& h, Input& in, astl::timeout timeout) noexcept : BasicPollAwaitable(h, in, framev, timeout) {}
    FrameWithStatus await_resume() noexcept(!E) {
        auto ret = FrameWithStatus(std::move(framev), this->status);
        if constexpr (E) {
            IoClosed::throwInputIfClosed(this->status);
        }
        return ret;
    }
    Frame framev;
};

template <bool E = true>
struct PollRef : public impl::BasicPollAwaitable {
    using BasicPollAwaitable::BasicPollAwaitable;
    io::status await_resume() noexcept(!E) {
        if constexpr (E) {
            IoClosed::throwInputIfClosed(this->status);
        }
        return this->status;
    }
};

template <bool E = true>
struct Push : public impl::BasicPushAwaitable {
    using BasicPushAwaitable::BasicPushAwaitable;
    io::status await_resume() noexcept(!E) {
        if constexpr (E) {
            IoClosed::throwOutputIfClosed(this->status);
        }
        return this->status;
    }
};

} // namespace coro

namespace impl {
template <typename T, typename Self>
struct SessionCoroPromiseHelper {
    void return_value(T value) noexcept {
        auto& self = static_cast<Self&>(*this);
        assert(self.m_result); // can't return value without a result to store it in
        *self.m_result = std::move(value);
        self.popCoro();
    }
};
template <typename Self>
struct SessionCoroPromiseHelper<void, Self> {
    void return_void() noexcept {
        auto& self = static_cast<Self&>(*this);
        if (self.m_result) {
            // m_result may be null in the root coroutine if it's void
            *self.m_result = {};
        }
        self.popCoro();
    }
};
} // namespace impl


template <typename T>
struct SessionCoro {
public:
    struct promise_type;
    using Handle = std::coroutine_handle<promise_type>;

    struct promise_type : public impl::SessionCoroPromiseHelper<T, promise_type> {
        using result_type = T;

        SessionCoro get_return_object() noexcept {
            return SessionCoro{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }

        void popCoro() noexcept {
            m_handler->setCoro(m_prev);

            if (m_prev) {
                m_handler->postResume();
            }
        }

        void unhandled_exception() noexcept {
            if (!m_result) {
                std::terminate(); // can't throw exceptions from the top coroutine
            }
            *m_result = astl::unexpected(std::current_exception());
            popCoro();
        }

        CoroSessionHandlerPtr m_handler;
        std::coroutine_handle<> m_prev = nullptr;

        // points to the result in the awaitable which is on the stack
        // null if this is the top coroutine
        coro::Result<T>* m_result = nullptr;
    };

    SessionCoro(SessionCoro&& other) noexcept : m_handle(std::exchange(other.m_handle, nullptr)) {}
    ~SessionCoro() {
        if (m_handle) {
            m_handle.destroy();
        }
    }

    Handle takeHandle() {
        return std::exchange(m_handle, nullptr);
    }

    struct CoroAwaitable {
        using Ret = T;
        using PromiseType = typename SessionCoro<T>::promise_type;
        std::coroutine_handle<PromiseType> coro;

        CoroAwaitable(std::coroutine_handle<PromiseType> h) noexcept : coro(h) {}

        // instead of making optional of expected, we can use the value error=nullptr to indicate that
        // the result is empty (hacky, but works)
        coro::Result<Ret> result = astl::unexpected();

        bool await_ready() const noexcept { return false; }

        template <typename CallerPromise>
        std::coroutine_handle<> await_suspend(std::coroutine_handle<CallerPromise> caller) noexcept {
            auto handler = caller.promise().m_handler;
            assert(handler->m_currentCoro = caller);
            coro.promise().m_result = &result;
            coro.promise().m_handler = handler;
            coro.promise().m_prev = caller;
            handler->setCoro(coro);
            return coro;
        }

        Ret await_resume() noexcept(false) {
            if (result) {
                return std::move(result).value();
            }
            else {
                std::rethrow_exception(result.error());
            }
        }
    };

    CoroAwaitable operator co_await() {
        return {takeHandle()};
    }

private:
    Handle m_handle;
    SessionCoro(Handle handle) noexcept : m_handle(handle) {}
};

namespace coro {

namespace impl {
class BasicIo {
protected:
    CoroSessionHandlerPtr m_handler;

    Input* m_input = nullptr;
    Output* m_output = nullptr;
public:

    // sync (eager) operations

    template <bool E = true>
    [[nodiscard]] io::status getFrame(Frame& frame) noexcept(E) {
        auto ret = m_handler->getFrame(*m_input, frame);
        if constexpr (E) {
            IoClosed::throwInputIfClosed(ret);
        }
        return ret;
    }
    template <bool E = true>
    [[nodiscard]] FrameWithStatus getFrame() noexcept(E) {
        FrameWithStatus ret;
        ret.s() = getFrame<E>(ret.value);
        return ret;
    }
    template <bool E = true>
    [[nodiscard]] io::status putFrame(Frame& frame) noexcept(E) {
        auto ret = m_handler->putFrame(*m_output, frame);
        if constexpr (E) {
            IoClosed::throwOutputIfClosed(ret);
        }
        return ret;
    }
    template <bool E = true>
    [[nodiscard]] io::status putFrame(Frame&& frame) noexcept(E) {
        return putFrame<E>(frame);
    }

    // async (suspending) operations

    template <bool E = true>
    [[nodiscard]] Poll<E> pollFrame(astl::timeout timeout = astl::timeout::never()) noexcept {
        return {m_handler, *m_input, timeout};
    }
    template <bool E = true>
    [[nodiscard]] PollRef<E> pollFrame(Frame& frame, astl::timeout timeout = astl::timeout::never()) noexcept {
        return {m_handler, *m_input, frame, timeout};
    }
    template <bool E = true>
    [[nodiscard]] Push<E> pushFrame(Frame& frame, astl::timeout timeout = astl::timeout::never()) noexcept {
        return {m_handler, *m_output, frame, timeout};
    }
    template <bool E = true>
    [[nodiscard]] Push<E> pushFrame(Frame&& frame, astl::timeout timeout = astl::timeout::never()) noexcept {
        return {m_handler, *m_output, frame, timeout};
    }
};
} // namespace impl

class AC_FRAME_EXPORT AttachedIo : public impl::BasicIo {
    // optional input/ouput payload (will be null if pointing within the session)
    InputPtr m_inputPl;
    OutputPtr m_outputPl;
public:
    AttachedIo(CoroSessionHandlerPtr handler, StreamEndpoint ep);
    ~AttachedIo();

    AttachedIo(AttachedIo&&) noexcept;
    AttachedIo& operator=(AttachedIo&&) noexcept;
};

class Io : public impl::BasicIo {
public:
    [[nodiscard]] AC_FRAME_EXPORT AttachedIo attach(StreamEndpoint ep);

    // awaitable interface
    bool await_ready() const noexcept { return false; }
    template <typename PromiseType>
    bool await_suspend(std::coroutine_handle<PromiseType> h) noexcept {
        assert(!m_handler);
        m_handler = h.promise().m_handler;
        m_input = &m_handler->shInput();
        m_output = &m_handler->shOutput();
        return false;
    }
    Io await_resume() noexcept { return std::move(*this); }
};

} // namespace coro

} // namespace ac::frameio
