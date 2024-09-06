// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ahttp/ahttp.hpp>
#include <doctest/doctest.h>

TEST_CASE("supports_url") {
    CHECK(ahttp::supports_url("http://example.com"));
    CHECK(ahttp::supports_https() == ahttp::supports_url("https://example.com"));
    CHECK_FALSE(ahttp::supports_url("git://example.com"));
    CHECK_FALSE(ahttp::supports_url("asdf"));
}
