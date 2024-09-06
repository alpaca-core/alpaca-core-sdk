// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <dl/FileDownload.hpp>
#include <doctest/doctest.h>

TEST_CASE("supportsUrl") {
    CHECK(dl::supportsUrl("http://example.com"));
    CHECK(dl::supportsHttps() == dl::supportsUrl("https://example.com"));
    CHECK_FALSE(dl::supportsUrl("git://example.com"));
    CHECK_FALSE(dl::supportsUrl("asdf"));
}
