// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../export.h"
#include "SessionHandlerPtr.hpp"
#include "SessionHandler.hpp"
#include "FrameWithStatus.hpp"
#include <astl/expected.hpp>
#include <astl/timeout.hpp>
#include <coroutine>
#include <stdexcept>

namespace ac::frameio {

template <typename T>
struct SessionCoro;

class CoroSessionHandler;
using CoroSessionHandlerPtr = std::shared_ptr<CoroSessionHandler>;

class AC_FRAME_EXPORT CoroSessionHandler final : public SessionHandler {
public:
    CoroSessionHandler() noexcept = default;
    ~CoroSessionHandler();

    static SessionHandlerPtr create(SessionCoro<void> coro);

    void postResume();
    void pollFrame(Frame& frame, Status& status, astl::timeout timeout) noexcept;
    void pushFrame(Frame& frame, Status& status, astl::timeout timeout) noexcept;
    void close();
private:
    template <typename T>
    friend struct SessionCoro;

    using CoroResult = astl::expected<std::any, std::exception_ptr>;

    std::coroutine_handle<> m_currentCoro;

    template <typename P>
    static std::coroutine_handle<> pushCoro(const CoroSessionHandlerPtr& self, std::coroutine_handle<P> next) {
        next.promise().m_handler = self;
        next.promise().m_prev = self->m_currentCoro;
        return std::exchange(self->m_currentCoro, next);
    }

    template <typename P>
    void popCoro() {
        auto cur = std::coroutine_handle<P>::from_address(m_currentCoro.address());
        m_currentCoro = cur.promise().m_prev;

        if (m_currentCoro) {
            postResume();
        }
        else {
            close();
        }
    }

    virtual void shConnected() noexcept override;
};

namespace coro {

struct AC_FRAME_EXPORT IoClosed : public std::runtime_error {
    using std::runtime_error::runtime_error;
    ~IoClosed();
};

namespace impl {
struct BasicFrameAwaitable {
    CoroSessionHandlerPtr handler;
    Frame* frame;
    astl::timeout timeout;
    Status status;

    BasicFrameAwaitable(const CoroSessionHandlerPtr& h, Frame& f, astl::timeout t) noexcept : handler(h), frame(&f), timeout(t) {}
    bool await_ready() const noexcept { return false; }
};

struct BasicPollAwaitable : public BasicFrameAwaitable {
    using BasicFrameAwaitable::BasicFrameAwaitable;
    void await_suspend(std::coroutine_handle<>) noexcept {
        this->handler->pollFrame(*this->frame, this->status, this->timeout);
    }
};

struct BasicPushAwaitable : public BasicFrameAwaitable {
    using BasicFrameAwaitable::BasicFrameAwaitable;
    void await_suspend(std::coroutine_handle<>) noexcept {
        this->handler->pushFrame(*this->frame, this->status, this->timeout);
    }
};
} // namespace impl

template <bool E = true>
struct Poll: public impl::BasicPollAwaitable {
    Poll(const CoroSessionHandlerPtr& h, astl::timeout timeout) noexcept : BasicPollAwaitable(h, framev, timeout) {}
    FrameWithStatus await_resume() noexcept(!E) {
        auto ret = FrameWithStatus(std::move(framev), this->status);
        if constexpr (E) {
            if (this->status.closed()) {
                throw IoClosed("input closed");
            }
        }
        return ret;
    }
    Frame framev;
};

template <bool E = true>
struct PollRef : public impl::BasicPollAwaitable {
    using BasicPollAwaitable::BasicPollAwaitable;
    Status await_resume() noexcept(!E) {
        if constexpr (E) {
            if (this->status.closed()) {
                throw IoClosed("input closed");
            }
        }
        return this->status;
    }
};

template <bool E = true>
struct Push : public impl::BasicPushAwaitable {
    using BasicPushAwaitable::BasicPushAwaitable;
    Status await_resume() noexcept(!E) {
        if constexpr (E) {
            if (this->status.closed()) {
                throw IoClosed("output closed");
            }
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
        if (self.m_result) {
            *self.m_result = std::move(value);
        }
        self.popCoro();
    }
};
template <typename Self>
struct SessionCoroPromiseHelper<void, Self> {
    void return_void() noexcept {
        auto& self = static_cast<Self&>(*this);
        if (self.m_result) {
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
            m_handler->popCoro<promise_type>();
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
        astl::expected<T, std::exception_ptr>* m_result = nullptr;
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
        astl::expected<Ret, std::exception_ptr> result = astl::unexpected();

        bool await_ready() const noexcept { return false; }

        template <typename CallerPromise>
        std::coroutine_handle<> await_suspend(std::coroutine_handle<CallerPromise> h) noexcept {
            coro.promise().m_result = &result;
            CoroSessionHandler::pushCoro(h.promise().m_handler, coro);
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

struct Io {
    CoroSessionHandlerPtr handler;

    [[nodiscard]] Poll<> pollFrame(astl::timeout timeout = astl::timeout::never()) noexcept {
        return {handler, timeout};
    }
    [[nodiscard]] PollRef<> pollFrame(Frame& frame, astl::timeout timeout = astl::timeout::never()) noexcept {
        return {handler, frame, timeout};
    }
    [[nodiscard]] Push<> pushFrame(Frame& frame, astl::timeout timeout = astl::timeout::never()) noexcept {
        return {handler, frame, timeout};
    }
    [[nodiscard]] Push<> pushFrame(Frame&& frame, astl::timeout timeout = astl::timeout::never()) noexcept {
        return {handler, frame, timeout};
    }

    // awaitable interface
    bool await_ready() const noexcept { return false; }
    template <typename PromiseType>
    bool await_suspend(std::coroutine_handle<PromiseType> h) noexcept {
        assert(!handler);
        handler = h.promise().m_handler;
        return false;
    }
    Io await_resume() noexcept { return *this; }
};

} // namespace coro

} // namespace ac::frameio
