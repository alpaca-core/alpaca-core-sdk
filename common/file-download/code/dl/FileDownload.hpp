// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <itlib/generator.hpp>
#include <string_view>
#include <vector>

namespace dl {

FILE_DOWNLOAD_EXPORT bool supportsUri(std::string_view uri) noexcept;

struct Chunk {
    size_t size = 0; // total size of the file

    size_t offset = 0; // offset of the chunk

    // ref to vector, you can take it or copy it
    std::vector<uint8_t>& data;
};

// initiate a synchronous file download
// returns a generator that yields chunks of the file
// the first chunk is always of size 0 and contains the total size of the file
// optionally provide chunkSize to control the size of the chunks (all but the last one will be of this size)
// chunkSize -1 means to try to download the whole file in one chunk
// note that if the server does provide size in the respose, the chunkSize will default to 1mb
FILE_DOWNLOAD_EXPORT itlib::generator<Chunk> downloadFileSync(std::string_view uri, size_t chunkSize = size_t(-1));

} // namespace dl
