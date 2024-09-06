// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ahttp.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
//#include <boost/beast/ssl.hpp>

#include <string> // must be here (workardound for furi bug)
#include <furi/furi.hpp>

#include <astl/throw_ex.hpp>
#include <astl/move.hpp>
#include <astl/iile.h>

#include <limits>
#include <iostream>

namespace net = boost::asio;
namespace ssl = net::ssl;
namespace beast = boost::beast;
namespace http = boost::beast::http;

namespace ahttp {

bool supports_https() noexcept {
    return false;
}

bool supports_url(std::string_view url) noexcept {
    auto scheme = furi::uri_split::get_scheme_from_uri(url);
    if (scheme == "http") return true;
    return false;
}

namespace {
template <typename Stream>
struct sync_state {
    std::shared_ptr<net::io_context> ctx;
    Stream stream;
    beast::flat_buffer buf;
    http::response_parser<http::buffer_body> parser;
    const size_t chunk_size;

    sync_state(std::shared_ptr<net::io_context> ctx, Stream stream, size_t chunk_size)
        : ctx(astl::move(ctx))
        , stream(astl::move(stream))
        , chunk_size(chunk_size)
    {
        parser.body_limit(std::numeric_limits<std::uint64_t>::max());
    }
};

template <typename Stream>
itlib::generator<chunk> get_sync_coro(std::unique_ptr<sync_state<Stream>> state) {
    const auto total_size = iile([&]() -> std::optional<size_t> {
        // silly convert boost::optional to std::optional
        auto ret = state->parser.content_length();
        if (ret) return *ret;
        return {};
    });

    auto chunk_size = state->chunk_size;
    if (total_size) {
        if (chunk_size > *total_size) {
            chunk_size = *total_size;
        }
    }
    else {
        if (chunk_size == std::numeric_limits<size_t>::max()) {
            chunk_size = 1024 * 1024; // 1mb
        }
    }

    std::vector<uint8_t> chunk_buf;
    size_t offset = 0;
    co_yield chunk{total_size, offset, chunk_buf};

    auto& body = state->parser.get().body();
    while (!state->parser.is_done()) {
        chunk_buf.resize(chunk_size);
        body.data = chunk_buf.data();
        body.size = chunk_buf.size();
        beast::error_code ec;
        http::read(state->stream, state->buf, state->parser, ec);

        if (ec && ec != http::error::need_buffer) {
            throw boost::system::system_error{ ec };
        }

        chunk_buf.resize(chunk_size - body.size);
        co_yield chunk{total_size, offset, chunk_buf};
        offset += chunk_buf.size();
    }
}

itlib::generator<chunk> get_sync(std::shared_ptr<net::io_context> ctx, std::string_view url, size_t chunk_size);

template <typename Stream>
itlib::generator<chunk> get_sync_t(std::unique_ptr<sync_state<Stream>> state, const http::request<http::empty_body>& req) {
    http::write(state->stream, req);

    state->parser.body_limit(std::numeric_limits<std::uint64_t>::max());

    http::read_header(state->stream, state->buf, state->parser);
    auto& header = state->parser.get().base();
    std::cout << header << std::endl;

    if (auto f = header.find(http::field::location); f != header.end()) {
        // redirect... so recurse
        auto location = f->value();
        std::cout << "redirecting to: " << location << std::endl;
        return get_sync(state->ctx, location, state->chunk_size);
    }

    return get_sync_coro(astl::move(state));
}

itlib::generator<chunk> get_sync(std::shared_ptr<net::io_context> ctx, std::string_view url, size_t chunk_size) {
    auto splitUrl = furi::uri_split::from_uri(url);

    http::request<http::empty_body> req;
    req.method(http::verb::get);
    req.target(splitUrl.req_path);
    req.keep_alive(true);
    req.set(http::field::host, splitUrl.authority);
    req.set(http::field::user_agent, "ac-file-download/1.0.0");
    req.set(http::field::accept, "*/*");
    // range support will be left for the future
    //req.set(http::field::range, "1-5");
    //std::cout << req << std::endl;

    beast::tcp_stream stream(*ctx);

    auto resolved = [&]() {
        net::ip::tcp::resolver resolver(*ctx);
        return resolver.resolve(splitUrl.authority, splitUrl.scheme);
    }();
    stream.connect(resolved);

    if (splitUrl.scheme == "http") {
        auto state = std::make_unique<sync_state<beast::tcp_stream>>(ctx, astl::move(stream), chunk_size);
        return get_sync_t(astl::move(state), req);
    }

    ac::throw_ex{} << "unsupported scheme: " << splitUrl.scheme;

    // unreachable, but some compilers failt to see that
    // C++23: replace with std::unreachable();
    std::terminate();
}
}

itlib::generator<chunk> get_sync(std::string_view url, size_t chunk_size) {
    return get_sync(std::make_shared<net::io_context>(), url, chunk_size);
}

} // namespace dl
