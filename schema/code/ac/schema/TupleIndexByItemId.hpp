// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <tuple>
#include <array>
#include <string_view>

namespace ac::local::schema::impl {

template <typename Tuple>
static constexpr int getTupleIndexByItemId(std::string_view id) {
    Tuple items;
    return std::apply([&](auto&&... item) -> int {
        int i = 0;
        std::array ids = {item.id...};
        for (auto n : ids) {
            if (n == id) {
                return i;
            }
            ++i;
        }
        return -1;
    }, items);
}

} // namespace ac::local::schema::impl
