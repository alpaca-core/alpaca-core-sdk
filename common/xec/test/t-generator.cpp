// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/xec/co_for.hpp>
#include <ac/xec/co_execute.hpp>
#include <doctest/doctest.h>
#include <span>
#include <vector>
#include <string>

using namespace ac::xec;

generator<int> range(int begin, int end) {
    // this absolutely pointless vector here is to trigger clang's ridiculous handling of coroutines
    // if we simply rethrow in unhandled_exception(), for some reason it calls the destructors of locals twice
    // having a local whose destructor is not safe to call twice will cause a crash
    // if we don't crash here on clang, then generator works as expected
    std::vector<int> store;
    for (int i = begin; i < end; ++i) {
        store.emplace_back(i);
    }

    for (auto i : store) {
        if (i == 103) throw std::runtime_error("test exception");
        co_yield i;
    }
}

coro<void> test_simple() {

    int i = 50;

    // range for
    co_for (x, range(i, i+10)) {
        CHECK(*x == i);
        ++i;
    }
    CHECK(i == 60);

    // next
    auto r = range(1, 5);
    CHECK(*co_await r.next() == 1);
    CHECK_FALSE(r.done());
    CHECK(*co_await r.next() == 2);
    CHECK(*co_await r.next() == 3);
    CHECK(*co_await r.next() == 4);
    CHECK_FALSE(co_await r.next());
    CHECK(r.done());

    // exceptions

    i = 0;
    CHECK_THROWS_WITH_AS(
        co_await [&]() -> coro<void> {
            co_for (x, range(100, 105)) {
                i = *x;
            }
        }(),
        "test exception",
        std::runtime_error
    );
    CHECK(i == 102);

    r = range(101, 105);
    CHECK_NOTHROW(co_await r.next());
    CHECK_NOTHROW(co_await r.next());
    CHECK_THROWS_WITH_AS(co_await r.next(), "test exception", std::runtime_error);

    CHECK(r.done());
}

TEST_CASE("simple") {
    co_execute(test_simple());
}

template <typename T>
generator<T&> ref_gen(std::span<T> vals) {
    for (T& v : vals) {
        co_yield v;
    }
}

coro<void> test_ref() {
    std::vector<int> ints = { 1, 2, 3, 4, 5 };
    co_for(i, ref_gen(std::span(ints))) {
        *i += 10;
    }
    CHECK(ints == std::vector<int>{11, 12, 13, 14, 15});

    auto cg = ref_gen(std::span<const int>(ints));
    const int& a = *co_await cg.next();
    const int& b = *co_await cg.next();
    auto cgi = co_await make_coro_iterator(std::move(cg));
    for (; !cgi.done(); co_await cgi.next()) {
        CHECK(*cgi > 12);
        CHECK(*cgi < 16);
    }
    CHECK(cgi.done());
    CHECK(a == 11);
    CHECK(&a == ints.data());
    CHECK(b == 12);
    CHECK(&b == ints.data() + 1);
}

TEST_CASE("ref") {
    co_execute(test_ref());
}

struct non_copyable {
    non_copyable(std::string v) : value(std::move(v)) {}
    non_copyable(const non_copyable&) = delete;
    non_copyable& operator=(const non_copyable&) = delete;
    non_copyable(non_copyable&&) noexcept = default;
    non_copyable& operator=(non_copyable&&) noexcept = default;
    std::string value;
};

generator<non_copyable> non_copyable_range(int begin, int end) {
    for (int i = begin; i < end; ++i) {
        co_yield non_copyable(std::to_string(i));
    }
}

coro<void> ync_test() {
    int i = 10;
    co_for(mi, non_copyable_range(10, 15)) {
        auto elem = std::move(*mi);
        CHECK(elem.value == std::to_string(i));
        ++i;
    }
}

TEST_CASE("yield non copyable") {
    co_execute(ync_test());
}

generator<std::string, int> yield_strings(int begin, int end) {
    for (int i = begin; i < end; ++i) {
        co_yield std::to_string(i);
    }
    co_return end - begin;
}

coro<void> return_iter_test() {
    auto gen = co_await make_coro_iterator(yield_strings(10, 15));
    std::vector<std::string> values;
    int i = 10;
    for (; !gen.done(); co_await gen.next()) {
        CHECK(*gen == std::to_string(i));
        ++i;
    }
    CHECK(i == 15);
    CHECK(gen.rval() == 5);
}

TEST_CASE("return iter") {
    co_execute(return_iter_test());
}

coro<void> return_next_test() {
    auto gen = yield_strings(10, 13);
    CHECK(*co_await gen.next() == "10");
    CHECK(*co_await gen.next() == "11");
    CHECK(*co_await gen.next() == "12");
    auto last = co_await gen.next();
    CHECK_FALSE(last);
    CHECK(last.error() == 3);
    CHECK(gen.done());
}

TEST_CASE("return next") {
    co_execute(return_next_test());
}
