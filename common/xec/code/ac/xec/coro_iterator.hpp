// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "coro.hpp"
#include <type_traits>

namespace ac::xec {

template <typename Gen, typename Ret>
class coro_iterator {
public:
    using generator_t = generator<Gen, Ret>;
    using return_type = typename generator_t::return_type;
    using value_type = std::decay_t<Gen>;
    using reference = std::conditional_t<std::is_reference_v<Gen>, Gen, Gen&>;

    coro_iterator(generator_t&& g, astl::expected<Gen, Ret>&& initial_value)
        : g(std::move(g))
        , value(std::move(initial_value))
    {}

    coro<void> next() {
        value = co_await g.next();
    }

    reference operator*() {
        return *value;
    }

    decltype(auto) rval() {
        return value.error();
    }

    bool done() const noexcept {
        return g.done();
    }
private:
    generator_t g;
    astl::expected<Gen, Ret> value;
};

template <typename Gen, typename Ret>
coro<coro_iterator<Gen, Ret>> make_coro_iterator(generator<Gen, Ret>&& g) {
    auto initial = co_await g.next();
    co_return coro_iterator<Gen, Ret>(std::move(g), std::move(initial));
}

} // namespace ac::xec
