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

TEST_CASE("just get") {
    auto gen = ahttp::get_sync("http://httpbin.org/bytes/128?seed=42");
    auto init = gen.next();
    REQUIRE(init);
    CHECK(init->size.value_or(0) == 128);
    CHECK(init->offset == 0);
    CHECK(init->data.size() == 0);
}
