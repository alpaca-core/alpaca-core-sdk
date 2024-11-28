// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "TupleIndexByItemId.hpp"
#include <ac/DictFwd.hpp>
#include <astl/tuple_util.hpp>

namespace ac::local::schema {

template <typename Tuple, typename Dispatcher>
constexpr decltype(auto) dispatchById(std::string_view id, Dict& dict, Dispatcher&& disp) {
    Tuple tup{};
    return astl::tuple::switch_index(tup, impl::getTupleIndexByItemId<Tuple>(id), [&](auto elem) {
        using Elem = std::decay_t<decltype(elem)>;
        if constexpr (std::is_same_v<Elem, nullptr_t>) {
            return disp.on(id, dict);
        }
        else {
            return disp.on(elem, typename Elem::Params{});
        }
    });
}

} // namespace ac::local::schema
