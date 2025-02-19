// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "value_with_status.hpp"
#include "stream_op.hpp"
#include "notify_on_blocked.hpp"
#include "exception.hpp"
#include "concepts/xio_cb.hpp"
#include <ac/xec/wobj_concept.hpp>
#include <astl/timeout.hpp>
#include <memory>
#include <cassert>
#include <system_error>
#include <coroutine>

namespace ac::io {

template <stream_class Stream, xec::basic_wait_object_class Wobj>
class basic_xio {
public:
    template <typename ...Args>
    explicit basic_xio(std::unique_ptr<Stream> stream, Args&&... args)
        requires std::constructible_from<Wobj, Args...>
        : m_stream(std::move(stream))
        , m_wobj(std::forward<Args>(args)...)
    {}

    template <typename ...Args>
    explicit basic_xio(Args&&... args)
        requires std::constructible_from<Wobj, Args...>
        : basic_xio(std::unique_ptr<Stream>{}, std::forward<Args>(args)...)
    {}

    using stream_type = Stream;
    using value_type = typename stream_type::value_type;
    using executor_type = typename Wobj::executor_type;

    bool attached() const noexcept { return !!m_stream; }
    explicit operator bool() const noexcept { return attached(); }

    std::unique_ptr<Stream> attach(std::unique_ptr<Stream> stream) {
        return std::exchange(m_stream, std::move(stream));
    }

    std::unique_ptr<Stream> detach() {
        return attach(nullptr);
    }

    const executor_type& get_executor() const {
        return m_wobj.get_executor();
    }

    template <bool E = true>
    status sync_io(value_type& value) {
        auto ret = stream_op(*m_stream, value, nullptr);
        if constexpr (E) {
            stream_closed_error::throw_if_closed(ret);
        }
        return ret;
    }

    template <xio_cb_class<value_type> Cb>
    void async_io(value_type& value, Cb&& cb) requires xec::wait_object_class<Wobj> {
        if (init_async_io(value, std::forward<Cb>(cb))) {
            return;
        }

        m_wobj.wait([this, &value, cb = std::forward<Cb>(cb)]([[maybe_unused]] const std::error_code& ec) {
            status s = stream_op(*m_stream, value, nullptr);
            assert(ec);
            s.set_aborted();
            cb(value, s);
        });
    }

    template <xio_cb_class<value_type> Cb>
    void async_io(value_type& value, astl::timeout to, Cb&& cb) requires xec::timeout_wait_object_class<Wobj> {
        if (init_async_io(value, std::forward<Cb>(cb))) {
            return;
        }

        m_wobj.wait(to, [this, &value, cb = std::forward<Cb>(cb)](const std::error_code& ec) {
            status s = stream_op(*m_stream, value, nullptr);

            if (ec) {
                s.set_aborted();
            }
            else {
                s.set_timeout();
            }

            cb(value, s);
        });
    }

    void close() {
        m_stream->close();
    }

    template <bool E>
    struct [[nodiscard]] async_awaitable {
        basic_xio& io;
        value_type& value;
        astl::timeout to;
        status s;
        std::coroutine_handle<> hcoro;

        async_awaitable(basic_xio& io, value_type& value, astl::timeout to) noexcept
            : io(io)
            , value(value)
            , to(to)
        {}

        bool await_ready() noexcept { return false; }

        void await_suspend(std::coroutine_handle<> coro) {
            hcoro = coro;
            io.async_io(value, to, [this](value_type&, status s) {
                this->s = s;
                hcoro.resume();
            });
        }

        status await_resume() noexcept(!E) {
            if constexpr (E) {
                stream_closed_error::throw_if_closed(s);
            }
            return s;
        }
    };

protected:
    ~basic_xio() = default;
private:
    template <xio_cb_class<value_type> Cb>
    bool init_async_io(value_type& value, Cb&& cb) {
        status s = stream_op(*m_stream, value, notify_on_blocked(m_wobj));
        if (s.complete()) {
            post(m_wobj.get_executor(), [cb = std::forward<Cb>(cb), &value, s]() mutable {
                cb(value, s);
            });
            return true;
        }
        return false;
    }

    std::unique_ptr<Stream> m_stream;
    Wobj m_wobj;
};

template <read_stream_class ReadStream, xec::basic_wait_object_class Wobj>
class xinput final : public basic_xio<ReadStream, Wobj> {
public:
    using super = basic_xio<ReadStream, Wobj>;
    using super::basic_xio;

    using typename super::stream_type;
    using typename super::value_type;
    using typename super::executor_type;

    template <bool E>
    using async_awaitable = typename super::template async_awaitable<E>;

    template <bool E = true>
    status get(value_type& value) {
        return super::sync_io<E>(value);
    }

    template <bool E = true>
    value_with_status<value_type> get() {
        value_with_status<value_type> ret;
        ret.s() = get<E>(ret.value);
        return ret;
    }

    template <xio_cb_class<value_type> Cb>
    void poll(value_type& value, Cb&& cb) requires xec::wait_object_class<Wobj> {
        super::async_io(value, std::forward<Cb>(cb));
    }

    template <xio_cb_class<value_type> Cb>
    void poll(value_type& value, astl::timeout to, Cb&& cb) requires xec::timeout_wait_object_class<Wobj> {
        super::async_io(value, to, std::forward<Cb>(cb));
    }

    template <bool E = true>
    async_awaitable<E> poll(value_type& value, astl::timeout to = astl::timeout::never()) {
        return super::template async_awaitable<E>(*this, value, to);
    }

    template <bool E>
    struct [[nodiscard]] poll_value_awaitable : public async_awaitable<E> {
        value_with_status<value_type> result;

        poll_value_awaitable(xinput& io, astl::timeout to)
            : super::template async_awaitable<E>(io, result.value, to)
        {}

        value_with_status<value_type> await_resume() {
            result.s() = async_awaitable::await_resume();
            return std::move(result);
        }
    };

    template <bool E = true>
    poll_value_awaitable<E> poll(astl::timeout to = astl::timeout::never()) {
        return poll_value_awaitable<E>(*this, to);
    }
};

template <write_stream_class WriteStream, xec::basic_wait_object_class Wobj>
class xoutput final : public basic_xio<WriteStream, Wobj> {
public:
    using super = basic_xio<WriteStream, Wobj>;
    using super::basic_xio;

    using typename super::stream_type;
    using typename super::value_type;
    using typename super::executor_type;

    template <bool E>
    using async_awaitable = typename super::template async_awaitable<E>;

    template <bool E = true>
    status put(value_type& value) {
        return super::sync_io<E>(value);
    }

    template <bool E = true>
    status put(value_type&& value) {
        return put<E>(value);
    }

    template <xio_cb_class<value_type> Cb>
    void push(value_type& value, Cb&& cb) requires xec::wait_object_class<Wobj> {
        super::async_io(value, std::forward<Cb>(cb));
    }

    template <xio_cb_class<value_type> Cb>
    void push(value_type& value, astl::timeout to, Cb&& cb) requires xec::timeout_wait_object_class<Wobj> {
        super::async_io(value, to, std::forward<Cb>(cb));
    }

    template <bool E = true>
    async_awaitable<E> push(value_type& value, astl::timeout to = astl::timeout::never()) {
        return async_awaitable(*this, value, to);
    }

    template <bool E>
    struct [[nodiscard]] push_value_awaitable : public async_awaitable<E> {
        value_type value;

        push_value_awaitable(xoutput& io, astl::timeout to)
            : super::template async_awaitable<E>(io, value, to)
        {}
    };

    template <bool E = true>
    async_awaitable<E> push(value_type&& value, astl::timeout to = astl::timeout::never()) {
        return push_value_awaitable<E>(*this, to);
    }
};

} // namespace ac::io