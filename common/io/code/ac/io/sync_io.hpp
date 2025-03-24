// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "stream_endpoint.hpp"
#include "value_with_status.hpp"
#include "exception.hpp"
#include <ac/xec/context.hpp>
#include <ac/xec/timer_wobj.hpp>

namespace ac::io {

template <typename RS, typename WS>
class sync_io;

struct sync_io_ctx : private xec::context {
    void run_tasks() {
        poll();
    }

    using xec::context::make_strand;
    using xec::context::make_work_guard;

    xec::context& get_executor() {
        return *this;
    }

    ~sync_io_ctx() {
        run_tasks();
    }
};

template <typename RS, typename WS>
class sync_io {
public:
    using stream_endpoint_type = stream_endpoint<RS, WS>;
private:
    sync_io_ctx& m_ctx;
    stream_endpoint_type m_ep;
public:
    explicit sync_io(sync_io_ctx& ctx)
        : m_ctx(ctx)
    {}

    sync_io(stream_endpoint_type ep, sync_io_ctx& ctx)
        : m_ctx(ctx)
        , m_ep(std::move(ep))
    {}

    ~sync_io() {
        close();
    }

    using input_value_type = typename RS::value_type;
    using output_value_type = typename WS::value_type;

    template <bool E = true>
    status get(input_value_type& val) {
        m_ctx.run_tasks();
        auto ret = m_ep.read_stream->read(val, nullptr);
        if constexpr (E) {
            stream_closed_error::throw_if_closed(ret);
        }
        return ret;
    }

    template <bool E = true>
    value_with_status<input_value_type> get() {
        value_with_status<input_value_type> ret;
        ret.s() = get<E>(ret.value);
        return ret;
    }

    template <bool E = true>
    status put(output_value_type& val) {
        auto ret = m_ep.write_stream->write(val, nullptr);
        if constexpr (E) {
            stream_closed_error::throw_if_closed(ret);
        }
        m_ctx.run_tasks();
        return ret;
    }

    template <bool E = true>
    status put(output_value_type&& val) {
        return put<E>(val);
    }

    stream_endpoint_type attach(stream_endpoint_type ep) {
        return std::exchange(m_ep, std::move(ep));
    }

    stream_endpoint_type detach() {
        return attach({});
    }

    void close() {
        // close streams, but don't run tasks if nothing was closed
        bool work_done = false;

        if (m_ep.read_stream) {
            m_ep.read_stream->close();
            work_done = true;
        }
        if (m_ep.write_stream) {
            m_ep.write_stream->close();
            work_done = true;
        }
        if (work_done) {
            m_ctx.run_tasks();
        }
    }
};

} // namespace ac::io
