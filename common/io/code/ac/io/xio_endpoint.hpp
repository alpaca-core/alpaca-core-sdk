// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "xio.hpp"
#include "stream_endpoint.hpp"

namespace ac::io {

template <class ReadStream, class WriteStream, class Wobj, bool Except = true>
struct xio_endpoint : private xinput<ReadStream, Wobj>, private xoutput<WriteStream, Wobj>
{
    using input_type = xinput<ReadStream, Wobj>;
    using input_value_type = typename input_type::value_type;
    using output_type = xoutput<WriteStream, Wobj>;
    using output_value_type = typename output_type::value_type;
    using stream_endpoint_type = stream_endpoint<ReadStream, WriteStream>;
    using executor_type = typename Wobj::executor_type;

    template <typename... Args>
    xio_endpoint(stream_endpoint_type sep, Args&&... args)
        requires std::constructible_from<Wobj, Args...>
        : input_type(std::move(sep.read_stream), args...)
        , output_type(std::move(sep.write_stream), args...)
    {
        assert(input.get_executor() == output.get_executor());
    }

    template <typename... Args>
    xio_endpoint(Args&&... args)
        requires std::constructible_from<Wobj, Args...>
        : xio_endpoint(stream_endpoint_type{}, std::forward<Args>(args)...)
    {}

    input_type& input() { return *this; }
    const input_type& input() const { return *this; }
    output_type& output() { return *this; }
    const output_type& output() const { return *this; }

    stream_endpoint_type attach(stream_endpoint_type sep) {
        return {
            input.attach(std::move(sep.read_stream)),
            output.attach(std::move(sep.write_stream))
        };
    }

    stream_endpoint_type detach() {
        return attach({});
    }

    void close() {
        input.close();
        output.close();
    }

    const executor_type& get_executor() const {
        return input.get_executor();
    }

    using input_type::get;
    using input_type::poll;

    using output_type::put;
    using output_type::push;
};

} // namespace ac::io
