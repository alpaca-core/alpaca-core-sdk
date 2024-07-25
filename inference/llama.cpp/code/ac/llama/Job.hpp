// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Token.hpp"
#include "mem_ext.hpp"
#include <string>
#include <span>
#include <coroutine>

struct llama_context;

namespace ac::llama {
class Model;

class SessionCoroutine {
public:
    struct Prompt {};

    class promise_type {
        Token m_value = Token_Invalid;

        std::string m_currentPrompt, m_pendingPrompt;
    public:
        SessionCoroutine get_return_object() noexcept {
            return SessionCoroutine{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        Token value() const noexcept { return m_value; }

        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(Token value) noexcept {
            m_value = value;
            return {};
        }

        void return_void() noexcept {
            m_value = Token_Invalid;
        }
        void unhandled_exception() { throw; }

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
    };

    using Handle = std::coroutine_handle<promise_type>;

    SessionCoroutine(Handle handle) : m_handle(handle) {}
    ~SessionCoroutine() {
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
        return std::move(m_handle.promise().value());
    }

private:
    Handle m_handle;
};

class AC_LLAMA_EXPORT Job {
public:
    struct InitParams {
        uint32_t ctxSize = 0; // context size for the model (0 = maximum allowed by model)
        uint32_t batchSize = 2048; // logical batch size for prompt processing (may be silently truncated to ctxSize)
        uint32_t ubatchSize = 0; // physical batch size for prompt processing (0 = batchSize)
    };

    explicit Job(Model& model, InitParams params);
    ~Job();

    // do an empty model run to load model data in cache
    void warmup();

    struct SessionParams {
        bool conversation = false;

        int numTokensToPredict = -1;

        uint32_t gaFactor = 1; // group-attention factor
        uint32_t gaWidth = 512; // group-attention width

        // if true, the inference tries to extend the context by truncating previous tokens
        // only used if gaFactor == 1
        bool infiniteContext = true;
    };

    SessionCoroutine newSession(std::string initialPrompt, const SessionParams params);

private:
    Model& m_model;
    astl::c_unique_ptr<llama_context> m_lctx;

    bool m_hasActiveSession = false;
};

} // namespace ac::llama
