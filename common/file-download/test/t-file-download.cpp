// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <dl/FileDownload.hpp>
#include <doctest/doctest.h>

TEST_CASE("supportsUri") {
    CHECK(dl::supportsUri("http://example.com"));
}
