// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

#include "FileDownload.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
//#include <boost/beast/ssl.hpp>

#include <string> // must be here (workardound for furi bug)
#include <furi/furi.hpp>

#include <astl/throw_ex.hpp>
#include <astl/iile.h>

#include <limits>
#include <iostream>

namespace net = boost::asio;
namespace ssl = net::ssl;
namespace beast = boost::beast;
namespace http = boost::beast::http;

namespace dl {

bool supportsHttps() noexcept {
    return false;
}

bool supportsUrl(std::string_view url) noexcept {
    auto scheme = furi::uri_split::get_scheme_from_uri(url);
    if (scheme == "http") return true;
    return false;
}

namespace {
itlib::generator<Chunk> downloadSyncCoro(beast::tcp_stream& stream, beast::flat_buffer& buf, http::response_parser<http::buffer_body>& parser, size_t chunkSize) {
    const auto totalSize = iile([&]() -> std::optional<size_t> {
        auto ret = parser.content_length();
        if (ret) return *ret;
        return {};
    });

    if (totalSize) {
        if (chunkSize > *totalSize) {
            chunkSize = *totalSize;
        }
    }
    else {
        if (chunkSize == std::numeric_limits<size_t>::max()) {
            chunkSize = 1024 * 1024; // 1mb
        }
    }

    std::vector<uint8_t> chunk;
    size_t offset = 0;
    co_yield Chunk{totalSize, offset, chunk};

    auto& body = parser.get().body();
    while (!parser.is_done()) {
        chunk.resize(chunkSize);
        body.data = chunk.data();
        body.size = chunk.size();
        beast::error_code ec;
        http::read(stream, buf, parser, ec);

        if (ec && ec != http::error::need_buffer) {
            throw boost::system::system_error{ ec };
        }

        chunk.resize(chunkSize - body.size);
        co_yield Chunk{totalSize, offset, chunk};
        offset += chunk.size();
    }
}

itlib::generator<Chunk> downloadFileSync(net::io_context& ctx, std::string_view url, size_t chunkSize);

template <typename Stream>
itlib::generator<Chunk> downloadSync(net::io_context& ctx, Stream& stream, const http::request<http::empty_body>& req, size_t chunkSize) {
    http::write(stream, req);

    beast::flat_buffer buf;
    http::response_parser<http::buffer_body> parser;
    parser.body_limit(std::numeric_limits<std::uint64_t>::max());

    http::read_header(stream, buf, parser);
    auto& header = parser.get().base();
    std::cout << header << std::endl;

    if (auto f = header.find(http::field::location); f != header.end()) {
        // redirect... so recurse
        auto location = f->value();
        std::cout << "redirecting to: " << location << std::endl;
        return downloadFileSync(ctx, location, chunkSize);
    }

    return downloadSyncCoro(stream, buf, parser, chunkSize);
}

itlib::generator<Chunk> downloadFileSync(net::io_context& ctx, std::string_view url, size_t chunkSize) {
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

    beast::tcp_stream stream(ctx);

    auto resolved = [&]() {
        net::ip::tcp::resolver resolver(ctx);
        return resolver.resolve(splitUrl.authority, splitUrl.scheme);
    }();
    stream.connect(resolved);

    if (splitUrl.scheme == "http") {
        return downloadSync(ctx, stream, req, chunkSize);
    }

    ac::throw_ex{} << "unsupported scheme: " << splitUrl.scheme;

    // unreachable, but some compilers failt to see that
    // C++23: replace with std::unreachable();
    std::terminate();
}
}

itlib::generator<Chunk> downloadFileSync(std::string_view uri, size_t chunkSize) {
    net::io_context ctx;
    return downloadFileSync(ctx, uri, chunkSize);
}

} // namespace dl
