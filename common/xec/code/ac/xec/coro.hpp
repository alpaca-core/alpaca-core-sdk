// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "strand.hpp"
#include <astl/expected.hpp>
#include <coroutine>
#include <stdexcept>
#include <cassert>

namespace ac::xec {

template <typename T>
using coro_result = astl::expected<T, std::exception_ptr>;

namespace impl {

template <typename T, typename Self>
struct ret_promise_helper {
    void return_value(T value) noexcept {
        auto& self = static_cast<Self&>(*this);
        assert(self.m_result); // can't return value without a result to store it in
        *self.m_result = std::move(value);
    }
};
template <typename Self>
struct ret_promise_helper<void, Self> {
    void return_void() noexcept {
        auto& self = static_cast<Self&>(*this);
        if (self.m_result) {
            // m_result may be null in the root coroutine if it's void
            *self.m_result = {};
        }
    }
};

struct opt_transfer {
    std::coroutine_handle<> h;
    bool await_ready() const noexcept {
        // eager when no h
        return !h;
    }
    std::coroutine_handle<> await_suspend(std::coroutine_handle<>) noexcept {
        // resume with caller
        return h;
    }
    void await_resume() noexcept {}
};

} // namespace impl

template <typename Ret, typename Gen = std::nullptr_t>
struct [[nodiscard]] coro {
    using return_type = Ret;
    using gen_type = Gen;

    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    using result_type = coro_result<Ret>;
    using gen_result_type = astl::eoptional<Gen>;

    struct promise_type : impl::ret_promise_helper<Ret, promise_type> {
        coro get_return_object() noexcept {
            return coro{handle_type::from_promise(*this)};
        }

        std::suspend_always initial_suspend() noexcept { return {}; }

        impl::opt_transfer final_suspend() noexcept {
            // what we do here is that we make the top coroutine have an eager return (not-suspending)
            // the top coroutine must be detached from its object (on co_spawn)
            // the others wont: they will be suspended here and destroyed from the caller
            return {m_prev};
        }

        void unhandled_exception() noexcept {
            if (!m_result) {
                std::terminate(); // can't throw exceptions from a naked top coroutine
            }
            *m_result = astl::unexpected(std::current_exception());
        }

        impl::opt_transfer yield_value(Gen value) noexcept {
            if (m_generated) {
                if constexpr (std::is_reference_v<Gen>) {
                    m_generated->emplace(value);
                }
                else {
                    m_generated->emplace(std::move(value));
                }
                return {m_prev};
            }
            else {
                // we're not in a generator so we throw away the yielded value and resume
                return {};
            }
        }

        strand m_executor;
        std::coroutine_handle<> m_prev = nullptr;

        // the following point to the result in the awaitable which is on the stack

        result_type* m_result = nullptr; // null if this is the top coroutine
        gen_result_type* m_generated = nullptr; // null if not awaiting a generation
    };

    coro() noexcept = default;
    coro(coro&& other) noexcept : m_handle(other.take_handle()) {}
    coro& operator=(coro&& other) noexcept {
        if (m_handle) {
            m_handle.destroy();
        }
        m_handle = other.take_handle();
        return *this;
    }
    ~coro() {
        if (m_handle) {
            m_handle.destroy();
        }
    }

    [[nodiscard]] handle_type take_handle() noexcept {
        return std::exchange(m_handle, nullptr);
    }

    struct basic_awaitable {
        handle_type hcoro;

        basic_awaitable(handle_type h) noexcept : hcoro(h) {}

        // instead of making optional of expected, we can use the value error=nullptr to indicate that
        // the result is empty (hacky, but works and saves indirections)
        result_type result = astl::unexpected();

        bool await_ready() const noexcept { return false; }

        template <typename CallerPromise>
        std::coroutine_handle<> await_suspend(std::coroutine_handle<CallerPromise> caller) noexcept {
            hcoro.promise().m_result = &result;
            hcoro.promise().m_executor = caller.promise().m_executor;
            hcoro.promise().m_prev = caller;
            return hcoro;
        }
    };

    struct throwing_awaitable : public basic_awaitable {
        using basic_awaitable::basic_awaitable;

        Ret await_resume() noexcept(false) {
            if (this->result) {
                return std::move(this->result).value();
            }
            else {
                std::rethrow_exception(this->result.error());
            }
        }
    };

    throwing_awaitable operator co_await() {
        return {m_handle};
    }

    struct result_awaitable : public basic_awaitable {
        using basic_awaitable::basic_awaitable;
        result_type await_resume() noexcept {
            return std::move(this->result);
        }
    };

    [[nodiscard]] result_awaitable safe_result() {
        return {m_handle};
    }

    struct gen_awaitable {
        handle_type hcoro;
        gen_result_type gen = astl::unexpected();
        result_type result = astl::unexpected();

        gen_awaitable(handle_type h) noexcept : hcoro(h) {}

        bool await_ready() const noexcept { return false; }

        template <typename CallerPromise>
        std::coroutine_handle<> await_suspend(std::coroutine_handle<CallerPromise> caller) noexcept {
            hcoro.promise().m_result = &result;
            hcoro.promise().m_generated = &gen;
            hcoro.promise().m_executor = caller.promise().m_executor;
            hcoro.promise().m_prev = caller;
            return hcoro;
        }

        astl::expected<Gen, Ret> await_resume() noexcept(false) {
            if (gen) {
                return std::move(gen).value();
            }
            else if (result) {
                if constexpr (std::is_same_v<void, Ret>) {
                    return astl::unexpected();
                }
                else {
                    return astl::unexpected(std::move(result).value());
                }
            }
            else {
                std::rethrow_exception(result.error());
            }
        }
    };

    [[nodiscard]] gen_awaitable next() {
        return {m_handle};
    }

    bool done() const noexcept {
        return m_handle.done();
    }

    explicit operator bool() const noexcept {
        return !!m_handle;
    }

private:
    handle_type m_handle;
    coro(handle_type h) noexcept : m_handle(h) {}
};

// awaitable to get the coroutine's executor from the coroutine itself
// co_await executor{}
struct executor {
    strand m_executor;

    // awaitable interface
    bool await_ready() const noexcept { return false; }
    template <typename PromiseType>
    bool await_suspend(std::coroutine_handle<PromiseType> h) noexcept {
        m_executor = h.promise().m_executor;
        return false;
    }
    strand await_resume() noexcept { return std::move(m_executor); }
};

template <typename Gen, typename Ret = void>
using generator = coro<Ret, Gen>;

} // namespace ac::xec
