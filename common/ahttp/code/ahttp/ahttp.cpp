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

itlib::generator<chunk> get_sync(std::string_view url, size_t chunk_size) {
    net::io_context ctx;
    beast::flat_buffer buf;
    std::string redirect_url;

    // loop while redirecting
    while (true) {
        buf.clear();

        auto splitUrl = furi::uri_split::from_uri(url);

        auto stream = iile([&]() -> std::unique_ptr<beast::tcp_stream> {
            beast::tcp_stream init_stream(ctx);

            net::ip::tcp::resolver resolver(ctx);
            auto resolved = resolver.resolve(splitUrl.authority, splitUrl.scheme);
            init_stream.connect(resolved);

            if (splitUrl.scheme == "http") {
                return std::make_unique<beast::tcp_stream>(astl::move(init_stream));
            }
            else {
                ac::throw_ex{} << "unsupported scheme: " << splitUrl.scheme;
            }
        });

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

        http::write(*stream, req);

        http::response_parser<http::buffer_body> parser;
        parser.body_limit(std::numeric_limits<std::uint64_t>::max());

        http::read_header(*stream, buf, parser);
        auto& header = parser.get().base();
        std::cout << header << std::endl;

        auto f = header.find(http::field::location);
        if (f != header.end()) {
            // redirect
            // update url and loop again
            auto loc = f->value();
            if (loc.starts_with('/')) {
                // relative path
                std::string host{splitUrl.scheme};
                host += "://";
                host += splitUrl.authority;
                redirect_url = host;
                redirect_url += loc;
            }
            else {
                // absolute path
                redirect_url = std::string(loc);
            }

            url = redirect_url;
            std::cout << "redirecting to: " << url << std::endl;
            continue;
        }

        // no redirect
        // get the resource and return

        const auto total_size = iile([&]() -> std::optional<size_t> {
            // silly convert boost::optional to std::optional
            auto ret = parser.content_length();
            if (ret) return *ret;
            return {};
        });

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
        co_yield chunk{total_size, offset, chunk_buf}; // initial yield with empty buffer and total size

        auto& body = parser.get().body();
        while (!parser.is_done()) {
            chunk_buf.resize(chunk_size);
            body.data = chunk_buf.data();
            body.size = chunk_buf.size();
            beast::error_code ec;
            http::read(*stream, buf, parser, ec);

            if (ec && ec != http::error::need_buffer) {
                throw boost::system::system_error{ec};
            }

            chunk_buf.resize(chunk_size - body.size);
            co_yield chunk{total_size, offset, chunk_buf};
            offset += chunk_buf.size();
        }

        co_return;
    }
}

} // namespace dl
