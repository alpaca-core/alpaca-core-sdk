// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "FileDownload.hpp"
#include <furi/furi.hpp>

namespace dl {

bool supportsHttps() noexcept {
    return false;
}

bool supportsUrl(std::string_view url) noexcept {
    auto scheme = furi::uri_split::get_scheme_from_uri(url);
    if (scheme == "http") return true;
    return false;
}

} // namespace dl
