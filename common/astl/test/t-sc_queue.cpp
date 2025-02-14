// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <astl/sc_queue.hpp>
#include <doctest/doctest.h>
#include <vector>
#include <thread>

TEST_CASE("basic") {
    astl::sc_queue<int> a;
    CHECK_FALSE(a.try_pop().has_value());

    a.push(1);
    CHECK(a.try_pop().value() == 1);
    CHECK_FALSE(a.try_pop().has_value());

    a.push(2);
    a.push(3);
    CHECK(a.try_pop().value() == 2);
    CHECK(a.try_pop().value() == 3);
    CHECK_FALSE(a.try_pop().has_value());
}

TEST_CASE("mt") {
    astl::sc_queue<int> q;
    auto prod_a = [&] {
        for (int i = 0; i < 1000; ++i) {
            q.push(i);
        }
    };
    auto prod_b = [&] {
        for (int i = 1000; i < 2000; ++i) {
            q.push(i);
        }
    };

    std::vector<int> a, b;
    auto cons = [&] {
        while (a.size() + b.size() < 2000) {
            auto t = q.try_pop();
            if (!t) continue;
            if (t.value() < 1000) {
                a.push_back(t.value());
            }
            else {
                b.push_back(t.value() - 1000);
            }
        }
    };
    std::thread t1(prod_a);
    std::thread t2(prod_b);
    std::thread t3(cons);
    t1.join();
    t2.join();
    t3.join();

    auto check_0_999 = [](const std::vector<int>& vec) -> int {
        if (vec.size() != 1000) return 1000;
        for (int i = 0; i < 1000; ++i) {
            if (vec[i] != i) return i;
        }
        return -1;
    };
    CHECK(check_0_999(a) == -1);
    CHECK(check_0_999(b) == -1);
}
