// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Token.hpp"
#include "Sampler.hpp"
#include "ChatFormat.hpp"
#include "mem_ext.hpp"
#include <itlib/generator.hpp>
#include <string>
#include <span>

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

    void setPrompt(std::string_view prompt) {
        m_handle.promise().setPrompt(prompt);
    }

    Token next() {
        if (m_handle.done()) return Token_Invalid;
        m_handle.resume();
        return std::move(m_handle.promise().value());
    }

    class PseudoIterator {
        Handle m_handle;
    public:
        using value_type = Token;

        PseudoIterator() noexcept = default;
        PseudoIterator(Handle handle) noexcept : m_handle(handle) {}

        Token operator*() const noexcept { return m_handle.promise().value(); }
        PseudoIterator& operator++() {
            m_handle.resume();
            return *this;
        }

        bool operator==(std::default_sentinel_t) const noexcept { return m_handle.done(); }
    };

    PseudoIterator begin() {
        m_handle.resume();
        return PseudoIterator{m_handle};
    }
    std::default_sentinel_t end() noexcept { return {}; }

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

    explicit Job(Model& model, InitParams params = {});
    ~Job();

    // do an empty model run to load model data in cache
    void warmup();

    struct RunParams {
        bool conversation = false;

        int numTokensToPredict = -1;

        uint32_t gaFactor = 1; // group-attention factor
        uint32_t gaWidth = 512; // group-attention width

        // if true, the inference tries to extend the context by truncating previous tokens
        // only used if gaFactor == 1
        bool infiniteContext = true;
    };

    void setup(std::string_view prompt, const RunParams& params);
    void decode(std::string_view prompt);
    SessionCoroutine generate(uint32_t maxTokens = uint32_t(-1));

private:
    Model& m_model;
    astl::c_unique_ptr<llama_context> m_lctx;

    void tryExpandContext(std::span<const Token> tokens); // try to expand context to accommodate tokens

    // we should have a `const Token` span but llama_batch doesn't let us
    void doDecode(std::span<Token> tokens);

    std::string chatAddAndFormat(std::string role, std::string text);

    ChatFormat m_chatFmt;

    struct SessionData {
        RunParams params;
        Sampler sampler;
        std::vector<ChatMsg> chat;
        uint32_t numKeep = 0; // number of tokens to keep in the context in case we overflow
        uint32_t numPast = 0; // number of tokens in the context (that's prompts + generated)

        // group attention state
        uint32_t gaIndex = 0; // number of grouped KV tokens (only used if params.gaFactor > 1)

        bool initialized = false;
    };
    SessionData m_sessionData;
};

} // namespace ac::llama
