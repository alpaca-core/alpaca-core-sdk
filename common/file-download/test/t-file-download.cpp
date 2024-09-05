// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <dl/FileDownload.hpp>
#include <doctest/doctest.h>

TEST_CASE("supportsUri") {
    CHECK(dl::supportsUri("http://example.com"));
    CHECK(dl::supportsHttps() == dl::supportsUri("https://example.com"));
    CHECK_FALSE(dl::supportsUri("git://example.com"));
    CHECK_FALSE(dl::supportsUri("asdf"));
}
