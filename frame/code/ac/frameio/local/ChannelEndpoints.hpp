// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../StreamEndpoint.hpp"
#include <ac/io/channel_endpoints.hpp>

namespace ac::frameio {
struct ChannelEndpoints : public ac::io::channel_endpoints<ReadStream, WriteStream> {
    using super = ac::io::channel_endpoints<ReadStream, WriteStream>;

    ChannelEndpoints() = default;

    ChannelEndpoints(ChannelEndpoints&&) = default;
    ChannelEndpoints& operator=(ChannelEndpoints&&) = default;

    template <typename RS, typename WS>
    ChannelEndpoints(ac::io::channel_endpoints<RS, WS> endpoints)
        : ac::io::channel_endpoints<ReadStream, WriteStream>({
            {std::move(endpoints.ab.read_stream), std::move(endpoints.ab.write_stream)},
            {std::move(endpoints.ba.read_stream), std::move(endpoints.ba.write_stream)}
        })
    {}
};
} // namespace ac::frameio
