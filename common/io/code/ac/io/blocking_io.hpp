// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "stream_endpoint.hpp"
#include "value_with_status.hpp"
#include "xio_endpoint.hpp"
#include <ac/xec/atomic_cvar.hpp>
#include <ac/xec/wait_func_invoke.hpp>
#include <ac/xec/task.hpp>

namespace ac::io {

namespace impl {
struct immediate_executor {
    bool operator==(const immediate_executor&) const { return true; }
};
void post(immediate_executor, xec::task t) {
    t();
}
struct cvar_wobj final {
    xec::atomic_cvar& m_cvar;

    cvar_wobj(xec::atomic_cvar& cvar)
        : m_cvar(cvar)
    {}

    void notify_one() {
        m_cvar.notify_one();
    }

    using executor_type = impl::immediate_executor;
    impl::immediate_executor get_executor() const {
        return {};
    }

    template <xec::wait_func_class Cb>
    void wait(astl::timeout to, Cb&& cb) {
        auto notified = m_cvar.wait(to);
        if (notified) {
            xec::wait_func_invoke_cancelled(cb);
        }
        else {
            xec::wait_func_invoke_timeout(cb);
        }
    }
};
} // namespace impl

template <typename RS, typename WS>
class blocking_io;

class blocking_io_ctx {
    template <typename RS, typename WS>
    friend class blocking_io;
    xec::atomic_cvar m_cvar;
};

template <typename RS, typename WS>
class blocking_io : private xio_endpoint<RS, WS, impl::cvar_wobj> {
public:
    using super = xio_endpoint<RS, WS, impl::cvar_wobj>;

    explicit blocking_io(blocking_io_ctx& ctx)
        : super(ctx.m_cvar)
    {}

    blocking_io(stream_endpoint<RS, WS> endpoint, blocking_io_ctx& ctx)
        : super(std::move(endpoint), ctx.m_cvar)
    {}

    using input_value_type = typename RS::read_value_type;
    using output_value_type = typename WS::write_value_type;

    status poll(input_value_type& val, astl::timeout timeout = astl::timeout::never()) {
        status ret;
        super::poll(val, timeout, [&](input_value_type&, status s) {
            ret = s;
        });
        return ret;
    }
    value_with_status<input_value_type> poll(astl::timeout timeout = astl::timeout::never()) {
        value_with_status<input_value_type> ret;
        ret.s() = poll(ret.value, timeout);
        return ret;
    }

    status push(output_value_type& val, astl::timeout timeout = astl::timeout::never()) {
        status ret;
        super::push(val, timeout, [&](output_value_type&, status s) {
            ret = s;
        });
        return ret;
    }
    status push(output_value_type&& val, astl::timeout timeout = astl::timeout::never()) {
        return push(val, timeout);
    }

    using super::get;
    using super::put;

    using super::attach;
    using super::detach;
    using super::close;
};

} // namespace ac::io
