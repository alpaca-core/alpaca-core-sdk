// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "stream_endpoint.hpp"
#include "value_with_status.hpp"
#include "xio_endpoint.hpp"
#include <ac/xec/thread_wobj.hpp>
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
struct blocking_wobj : public xec::thread_wobj {
    using executor_type = immediate_executor;
    immediate_executor get_executor() const {
        return {};
    }

    template <xec::wait_func_class Cb>
    void wait(astl::timeout to, Cb&& cb) {
        auto notified = xec::thread_wobj::wait(to);
        if (notified) {
            xec::wait_func_invoke_cancelled(cb);
        }
        else {
            xec::wait_func_invoke_timeout(cb);
        }
    }
};
}

template <read_stream_class RS, write_stream_class WS>
class blocking_io {
public:
    explicit blocking_io(stream_endpoint<RS, WS> endpoint)
        : m_io(std::move(endpoint))
    {}

    using input_value_type = typename RS::read_value_type;
    using output_value_type = typename WS::write_value_type;

    status poll(input_value_type& val, astl::timeout timeout = astl::timeout::never()) {
        status ret;
        m_io.poll(val, timeout, [&](input_value_type&, status s) {
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
        m_io.push(val, timeout, [&](output_value_type&, status s) {
            ret = s;
        });
        return ret;
    }

    status push(output_value_type&& val, astl::timeout timeout = astl::timeout::never()) {
        return push(val, timeout);
    }

    void close() {
        m_io.close();
    }

private:
    xio_endpoint<RS, WS, impl::blocking_wobj> m_io;
};

} // namespace ac::io
