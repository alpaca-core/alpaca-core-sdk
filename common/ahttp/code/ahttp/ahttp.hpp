// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <itlib/generator.hpp>
#include <string_view>
#include <optional>
#include <cstdint>
#include <vector>

namespace ahttp {

AHTTP_EXPORT bool supports_https() noexcept;
AHTTP_EXPORT bool supports_url(std::string_view url) noexcept;

struct chunk {
    std::optional<size_t> size = 0; // total size of the file (may not be provided by the server)

    size_t offset = 0; // offset of the chunk

    // ref to vector, you can take it or copy it
    std::vector<uint8_t>& data;
};

// initiate a synchronous file download
// returns a generator that yields chunks of the file
// the first chunk is always of size 0 and contains the total size of the file
// optionally provide chunk_size to control the size of the chunks (all but the last one will be of this size)
// chunk_size -1 means to try to download the whole file in one chunk
// note that if the server does provide size in the respose, the chunk_size will default to 1mb
AHTTP_EXPORT itlib::generator<chunk> get_sync(std::string_view url, size_t chunk_size = size_t(-1));

} // namespace dl
