// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../export.h"
#include "SessionHandlerPtr.hpp"
#include "SessionHandler.hpp"
#include "FrameWithStatus.hpp"
#include <coroutine>
#include <stdexcept>
#include <astl/expected.hpp>

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
    void pollFrame(Frame& frame, Status& status, int32_t timeout) noexcept;
    void pushFrame(Frame& frame, Status& status, int32_t timeout) noexcept;
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

namespace impl {
struct BasicFrameAwaitable {
    CoroSessionHandlerPtr handler;
    Frame* frame;
    int32_t timeout;
    Status status;

    // never invoked, but allows AwaitableProxy to work
    BasicFrameAwaitable(const CoroSessionHandlerPtr& h, int32_t t) noexcept : handler(h), frame(nullptr), timeout(t) {}

    BasicFrameAwaitable(const CoroSessionHandlerPtr& h, Frame& f, int32_t t) noexcept : handler(h), frame(&f), timeout(t) {}
    bool await_ready() const noexcept { return false; }
};

struct BasicPollAwaitable : public BasicFrameAwaitable {
    using BasicFrameAwaitable::BasicFrameAwaitable;
    void await_suspend(std::coroutine_handle<>) noexcept {
        this->handler->pollFrame(*this->frame, this->status, this->timeout);
    }
};

template <bool E = true>
struct Poll: public BasicPollAwaitable {
    Poll(const CoroSessionHandlerPtr& h, int32_t timeout) noexcept : BasicPollAwaitable(h, framev, timeout) {}
    using BasicPollAwaitable::BasicPollAwaitable; // never invoked, but allows AwaitableProxy to work
    FrameWithStatus await_resume() noexcept(!E) {
        auto ret = FrameWithStatus(std::move(framev), this->status);
        if constexpr (E) {
            if (this->status.closed()) {
                throw std::runtime_error("input closed");
            }
        }
        return ret;
    }
    Frame framev;
};

template <bool E = true>
struct PollRef : public BasicPollAwaitable {
    using BasicPollAwaitable::BasicPollAwaitable;
    Status await_resume() noexcept(!E) {
        if constexpr (E) {
            if (this->status.closed()) {
                throw std::runtime_error("input closed");
            }
        }
        return this->status;
    }
};

struct BasicPushAwaitable : public BasicFrameAwaitable {
    using BasicFrameAwaitable::BasicFrameAwaitable;
    void await_suspend(std::coroutine_handle<>) noexcept {
        this->handler->pushFrame(*this->frame, this->status, this->timeout);
    }
};
template <bool E = true>
struct Push : public BasicPushAwaitable {
    using BasicPushAwaitable::BasicPushAwaitable;
    Status await_resume() noexcept(!E) {
        if constexpr (E) {
            if (this->status.closed()) {
                throw std::runtime_error("output closed");
            }
        }
        return this->status;
    }
};

template <typename PromiseType>
struct CoroAwaitable {
    std::coroutine_handle<PromiseType> coro;
    using Ret = typename PromiseType::result_type;

    CoroAwaitable(std::coroutine_handle<PromiseType> h) noexcept : coro(h) {}

    // instead of making optional of expected, we can use the value error=nullptr to indicate that
    // the result is empty (hacky, but works)
    astl::expected<Ret, std::exception_ptr> result = astl::unexpected();

    bool await_ready() const noexcept { return false; }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<>) noexcept {
        coro.promise().m_result = &result;
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

} // namespace impl

template <typename A>
struct AwaitableProxy {
    Frame* frame;
    int32_t timeout;
    AwaitableProxy(Frame& f, int32_t t) noexcept : frame(&f), timeout(t) {}
    AwaitableProxy(int32_t t) noexcept : frame(nullptr), timeout(t) {}

    A getAwaitable(const CoroSessionHandlerPtr& handler) noexcept {
        if (frame) {
            return A(handler, *frame, timeout);
        }
        return A(handler, timeout);
    }
};

template <bool E = true>
static AwaitableProxy<impl::Poll<E>> pollFrame(int32_t timeout) noexcept {
    return {timeout};
}
template <bool E = true>
static AwaitableProxy<impl::PollRef<E>> pollFrame(Frame& frame, int32_t timeout) noexcept {
    return {frame, timeout};
}
template <bool E = true>
static AwaitableProxy<impl::Push<E>> pushFrame(Frame& frame, int32_t timeout) {
    return {frame, timeout};
}
template <bool E = true>
static AwaitableProxy<impl::Push<E>> pushFrame(Frame&& frame, int32_t timeout) {
    return {frame, timeout};
}

} // namespace coro


template <typename T>
struct SessionCoro {
public:
    struct promise_type;
    using Handle = std::coroutine_handle<promise_type>;

    struct promise_type {
        using result_type = T;

        SessionCoro get_return_object() noexcept {
            return SessionCoro{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() noexcept {
            if (m_result) {
                *m_result = {};
            }
            m_handler->popCoro<promise_type>();
        }
        void unhandled_exception() noexcept {
            if (!m_result) {
                std::terminate(); // can't throw exceptions from the top coroutine
            }
            *m_result = astl::unexpected(std::current_exception());
            m_handler->popCoro<promise_type>();
        }

        CoroSessionHandlerPtr m_handler;
        std::coroutine_handle<> m_prev = nullptr;

        // points to the result in the awaitable which is on the stack
        // null if this is the top coroutine
        astl::expected<T, std::exception_ptr>* m_result = nullptr;

        template <typename Awaitable>
        Awaitable await_transform(coro::AwaitableProxy<Awaitable> proxy) {
            return proxy.getAwaitable(m_handler);
        }

        template <typename U>
        coro::impl::CoroAwaitable<typename SessionCoro<U>::promise_type> await_transform(SessionCoro<U> sc) {
            auto h = std::exchange(sc.m_handle, nullptr);
            // somewhat hacky, but pushCoro doesn't resume anything, so we have the stack being wrong
            // for a short while
            // immediately after this function returns, the current coroutine will be suspended and the stack will be
            // correct again
            CoroSessionHandler::pushCoro(m_handler, h);
            return {h};
        }
    };

    SessionCoro(SessionCoro&& other) noexcept : m_handle(std::exchange(other.m_handle, nullptr)) {}
    ~SessionCoro() {
        if (m_handle) {
            m_handle.destroy();
        }
    }

private:
    friend class CoroSessionHandler;
    Handle m_handle;
    SessionCoro(Handle handle) noexcept : m_handle(handle) {}
};

} // namespace ac::frameio
