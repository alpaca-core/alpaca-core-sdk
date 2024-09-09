// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <span>
#include <cstdint>
#include <utility>
#include <cassert>
#include <optional>
#include <coroutine>

namespace ahttp {

class sync_generator {
public:
    struct chunk_buf_t {};

    struct promise_type {
        std::optional<size_t> m_size;
        std::span<uint8_t> m_chunk_buf;

        // ah... exceptions and coroutines
        // so, what happens here?
        // this is an eager coroutine, which means that it will not suspend initially
        // if an exception is thrown before the first suspend, there is no way to properly propagate it to the caller
        // ...not when calling the coroutine function, at least
        // to throw it to the caller, we can employ a hack: immediately rethrow in unhandled_exception
        // this is generally a no-no as it will cause a leak of the coroutine buffer, but, as it turns out,
        // all compilers will not cause a leak if the coroutine has not been suspended yet
        // so, we rethrow if we've never been suspended, but store the exception an throw it later if we have
        // ew
        std::exception_ptr m_exception;
        bool m_suspended = false;
        bool m_has_exception_before_suspend = false;

        promise_type() noexcept = default;
        ~promise_type() = default;
        sync_generator get_return_object() noexcept {
            return sync_generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        // never suspend at until we yield the size
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }

        // yield the size of the resource
        std::suspend_never yield_value(size_t size) noexcept {
            assert(!m_size);
            m_size = size;
            return {};
        }

        // yield that there is no size available
        std::suspend_never yield_value(std::nullopt_t) noexcept {
            assert(!m_size);
            m_size = std::nullopt;
            return {};
        }

        void return_void() noexcept {}

        void unhandled_exception() {
            if (m_suspended) {
                m_exception = std::current_exception();
            }
            else {
                m_has_exception_before_suspend = true;
                throw;
            }
        }

        struct await_chunk_buf {
            std::span<uint8_t>& buf;
            bool await_ready() const noexcept { return false; }
            void await_suspend(std::coroutine_handle<promise_type>) noexcept {}
            std::span<uint8_t>& await_resume() noexcept { return buf; }
        };

        await_chunk_buf await_transform(chunk_buf_t) noexcept {
            m_suspended = true;
            return await_chunk_buf{m_chunk_buf};
        }
    };

    using handle_t = std::coroutine_handle<promise_type>;

    sync_generator(const sync_generator&) = delete;
    sync_generator& operator=(const sync_generator&) = delete;
    sync_generator(sync_generator&& other) noexcept : m_handle(std::exchange(other.m_handle, nullptr)) {}
    sync_generator& operator=(sync_generator&& other) noexcept {
        if (m_handle) m_handle.destroy();
        m_handle = std::exchange(other.m_handle, nullptr);
        return *this;
    }

    ~sync_generator() {
        // only destroy if there was no exception before the first suspend
        if (m_handle && !m_handle.promise().m_has_exception_before_suspend) {
            m_handle.destroy();
        }
    }

    const std::optional<size_t>& size() const noexcept {
        return m_handle.promise().m_size;
    }

    bool done() const noexcept {
        return m_handle.done();
    }

    // get the next chunk of the resource while providing a buffer to store it in
    // will return the input span, truncated to the size of the received chunk
    // it's safe to assume that each chunk except the last will be exactly the size of the buffer
    std::span<uint8_t> get_next_chunk(std::span<uint8_t> buffer) {
        if (done()) return {}; // do not resume past the final suspend
        auto& p = m_handle.promise();
        p.m_chunk_buf = buffer;
        m_handle.resume();
        if (p.m_exception) {
            std::rethrow_exception(p.m_exception);
        }
        return p.m_chunk_buf;
    }
private:
    handle_t m_handle = nullptr;
    explicit sync_generator(handle_t handle) noexcept : m_handle(handle) {}
};

} // namespace ahttp