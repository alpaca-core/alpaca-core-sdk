// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "transparent_string_hash.hpp"
#include <string>
#include <unordered_map>
// transparent string unordered map

namespace astl {

namespace impl {
template <typename A>
using string_type = std::basic_string<char, std::char_traits<char>, A>;
} // namespace impl

template <typename T,
    typename StringAllocator = std::allocator<char>,
    typename MapAllocator = std::allocator<std::pair<const impl::string_type<StringAllocator>, T>>
>
using tsumap = std::unordered_map<
    impl::string_type<StringAllocator>,
    T,
    transparent_string_hash,
    std::equal_to<>,
    MapAllocator
>;

} // namespace astl
