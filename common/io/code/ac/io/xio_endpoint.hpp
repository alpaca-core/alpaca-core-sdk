// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "xio.hpp"
#include "stream_endpoint.hpp"

namespace ac::io {

template <read_stream_class ReadStream, write_stream_class WriteStream, xec::basic_wait_object_class Wobj>
struct xio_endpoint {
    using input_type = xinput<ReadStream, Wobj>;
    using output_type = xoutput<WriteStream, Wobj>;
    using stream_endpoint_type = stream_endpoint<ReadStream, WriteStream>;
    using executor_type = typename Wobj::executor_type;

    input_type input;
    output_type output;

    template <typename... Args>
    xio_endpoint(stream_endpoint_type sep, Args&&... args)
        requires std::constructible_from<Wobj, Args...>
        : input(std::move(sep.read_stream), args...)
        , output(std::move(sep.write_stream), args...)
    {
        assert(input.get_executor() == output.get_executor());
    }

    template <typename... Args>
    xio_endpoint(Args&&... args)
        requires std::constructible_from<Wobj, Args...>
        : xio_endpoint(stream_endpoint_type{}, std::forward<Args>(args)...)
    {}

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
};

} // namespace ac::io
