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
    auto size = gen.size();
    CHECK(size.value_or(0) == 128);
}

TEST_CASE("chunked get") {
    auto gen = ahttp::get_sync("http://httpbin.org/bytes/512?seed=42");
    auto size = gen.size();
    CHECK(size.value_or(0) == 512);
}

TEST_CASE("redirect once") {
    auto gen = ahttp::get_sync("http://httpbin.org/redirect-to?url=http%3A%2F%2Fhttpbin.org%2Fbytes%2F64");
    auto size = gen.size();
    CHECK(size.value_or(0) == 64);
}
