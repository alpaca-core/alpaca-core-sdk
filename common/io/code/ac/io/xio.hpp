// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "status.hpp"
#include "stream_op.hpp"
#include "concepts/xio_cb.hpp"
#include <ac/xec/wobj_concept.hpp>
#include <astl/timeout.hpp>
#include <memory>
#include <cassert>
#include <system_error>

namespace ac::io {

template <stream_class Stream, xec::basic_wait_object_class Wobj>
class basic_xio {
public:
    template <typename ...Args>
    explicit basic_xio(Args&&... args)
        : m_wobj(std::forward<Args>(args)...)
    {}

    template <typename ...Args>
    explicit basic_xio(std::unique_ptr<Stream> stream, Args&&... args)
        : m_stream(std::move(stream))
        , m_wobj(std::forward<Args>(args)...)
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

    status sync_io(value_type& value) {
        return stream_op(*m_stream, value, nullptr);
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

protected:
    ~basic_xio() = default;
private:
    template <xio_cb_class<value_type> Cb>
    bool init_async_io(value_type& value, Cb&& cb) {
        status s = stream_op(*m_stream, value, &m_wobj);
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

    status get(value_type& value) {
        return super::sync_io(value);
    }

    template <xio_cb_class<value_type> Cb>
    void poll(value_type& value, Cb&& cb) requires xec::wait_object_class<Wobj> {
        super::async_io(value, std::forward<Cb>(cb));
    }

    template <xio_cb_class<value_type> Cb>
    void poll(value_type& value, astl::timeout to, Cb&& cb) requires xec::timeout_wait_object_class<Wobj> {
        super::async_io(value, to, std::forward<Cb>(cb));
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

    status put(value_type& value) {
        return super::sync_io(value);
    }

    template <xio_cb_class<value_type> Cb>
    void push(value_type& value, Cb&& cb) requires xec::wait_object_class<Wobj> {
        super::async_io(value, std::forward<Cb>(cb));
    }

    template <xio_cb_class<value_type> Cb>
    void push(value_type& value, astl::timeout to, Cb&& cb) requires xec::timeout_wait_object_class<Wobj> {
        super::async_io(value, to, std::forward<Cb>(cb));
    }
};

} // namespace ac::io