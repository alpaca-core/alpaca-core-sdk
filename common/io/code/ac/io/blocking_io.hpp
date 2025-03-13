// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "stream_endpoint.hpp"
#include "value_with_status.hpp"
#include "xio_endpoint.hpp"
#include <ac/xec/wait_func_invoke.hpp>
#include <ac/xec/task.hpp>

#include <condition_variable>
#include <mutex>

namespace ac::io {

namespace impl {
struct immediate_executor {
    bool operator==(const immediate_executor&) const { return true; }
};
void post(immediate_executor, xec::task t) {
    t();
}
class cvar_wobj {
    std::mutex m_mutex;
    bool m_flag = false;
    std::condition_variable m_cvar;

    bool wait_pred() {
        return std::exchange(m_flag, false);
    }

    bool just_wait(astl::timeout to) {
        std::unique_lock lock(m_mutex);
        if (to.is_infinite()) {
            m_cvar.wait(lock, [&] { return wait_pred(); });
            return true;
        }
        else {
            return m_cvar.wait_for(lock, to.duration, [&] { return wait_pred(); });
        }
    }
public:
    void notify_one() {
        std::lock_guard l(m_mutex);
        m_flag = true;
        // notify while mutex is locked
        // thus the destruction of the wobj won't race with spurious wakeups of the cvar
        m_cvar.notify_one();
    }

    using executor_type = impl::immediate_executor;
    impl::immediate_executor get_executor() const {
        return {};
    }

    template <xec::wait_func_class Cb>
    void wait(astl::timeout to, Cb&& cb) {
        auto notified = just_wait(to);
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
class blocking_io : private xio_endpoint<RS, WS, impl::cvar_wobj> {
public:
    using super = xio_endpoint<RS, WS, impl::cvar_wobj>;

    blocking_io() = default;

    explicit blocking_io(stream_endpoint<RS, WS> endpoint)
        : super(std::move(endpoint))
    {}

    using input_value_type = super::input_value_type;
    using output_value_type = super::output_value_type;

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
