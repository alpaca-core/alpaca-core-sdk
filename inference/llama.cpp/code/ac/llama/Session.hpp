// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Token.hpp"
#include <string>
#include <utility>
#include <exception>
#include <coroutine>

namespace ac::llama {

class Session {
public:
    struct Prompt {}; // sentinel for co_await

    class promise_type {
        Token m_value = Token_Invalid;

        std::string m_currentPrompt, m_pendingPrompt;

        std::exception_ptr m_exception;
    public:
        Session get_return_object() noexcept {
            return Session{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        Token value() const noexcept { return m_value; }

        std::suspend_always initial_suspend() noexcept { return {}; } // should we always suspend here?
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(Token value) noexcept {
            m_value = value;
            return {};
        }

        void return_void() noexcept {
            m_value = Token_Invalid;
        }
        void unhandled_exception() noexcept {
            // why store the exception here instead of simply rethrowing?
            // because clang is stupid, that's why
            // clang's ridiculous handling of corouitnes causes local coroutine variables to be destroyed twice if we
            // just rethrow here
            m_exception = std::current_exception();
        }

        struct Awaiter {
            promise_type& self;
            bool await_ready() const noexcept { return true; }
            const std::string& await_resume() noexcept {
                std::swap(self.m_currentPrompt, self.m_pendingPrompt);
                self.m_pendingPrompt.clear();
                return self.m_currentPrompt;
            }
            void await_suspend(std::coroutine_handle<>) noexcept {}
        };

        void setPrompt(std::string_view prompt) {
            m_pendingPrompt = prompt;
        }

        Awaiter await_transform(Prompt) noexcept { return Awaiter{*this}; }

        void rethrowIfException() {
            if (m_exception) {
                std::rethrow_exception(m_exception);
            }
        }
    };

    using Handle = std::coroutine_handle<promise_type>;

    Session() = default;
    Session(Handle handle) : m_handle(handle) {}

    Session(Session&& other) noexcept : m_handle(std::exchange(other.m_handle, nullptr)) {}
    Session& operator=(Session&& other) noexcept {
        if (this == &other) return *this;
        if (m_handle) {
            m_handle.destroy();
        }
        m_handle = std::exchange(other.m_handle, nullptr);
        return *this;
    }

    ~Session() {
        if (m_handle) {
            m_handle.destroy();
        }
    }

    void reset() {
        if (m_handle) {
            m_handle.destroy();
        }
        m_handle = nullptr;
    }

    explicit operator bool() const noexcept { return !!m_handle; }

    void pushPrompt(std::string_view prompt) {
        m_handle.promise().setPrompt(prompt);
    }

    Token getToken() {
        if (m_handle.done()) return Token_Invalid;
        m_handle.resume();
        m_handle.promise().rethrowIfException();
        return std::move(m_handle.promise().value());
    }

private:
    Handle m_handle;
};

} // namespace ac::llama
