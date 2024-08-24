// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <astl/tsumap.hpp>
#include <doctest/doctest.h>

TEST_CASE("basic") {
    // test that it compiles

    astl::tsumap<int> m;
    m["hello"] = 42;
    CHECK(m["hello"] == 42);

    astl::tsumap<int, std::pmr::polymorphic_allocator<char>> m2;
    m2["hello"] = 42;
    CHECK(m2["hello"] == 42);
}

// create a class that can be implicitly converted to string_view, but not to string and test that it can be used
// to look up a value in a tsumap

class string_viewable {
    std::string_view m_sv;
public:
    string_viewable(const char* str) : m_sv(str) {}

    operator std::string_view() const { return m_sv; }
    operator std::string() const = delete;

    friend auto operator==(const string_viewable& lhs, const std::string& rhs) { return lhs.m_sv == rhs; }
    friend auto operator==(const std::string& lhs, const string_viewable& rhs) { return lhs == rhs.m_sv; }

};

TEST_CASE("lookup") {
    astl::tsumap<int> m;
    m["hello"] = 42;

    string_viewable sv = "hello";

    auto f = m.find(sv);
    CHECK(f != m.end());

    string_viewable sv2 = "world";
    auto f2 = m.find(sv2);
    CHECK(f2 == m.end());
}
