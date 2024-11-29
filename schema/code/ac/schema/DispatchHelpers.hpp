// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "TupleIndexByItemId.hpp"
#include "IOVisitors.hpp"
#include <ac/DictFwd.hpp>
#include <astl/tuple_util.hpp>

namespace ac::local::schema {

template <typename Tuple, typename Dispatcher>
constexpr decltype(auto) dispatchById(std::string_view id, Dict&& dict, Dispatcher&& disp) {
    Tuple tup{};
    return astl::tuple::switch_index(tup, impl::getTupleIndexByItemId<Tuple>(id), [&](auto elem) {
        using Elem = std::decay_t<decltype(elem)>;
        if constexpr (std::is_same_v<Elem, nullptr_t>) {
            return disp.on(id, astl::move(dict));
        }
        else {
            return disp.on(elem, Struct_fromDict<Elem::Params>(astl::move(dict)));
        }
    });
}

} // namespace ac::local::schema
