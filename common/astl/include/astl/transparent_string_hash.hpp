// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <string_view>

namespace astl {
struct transparent_string_hash : public std::hash<std::string_view> {
    using hash_type = std::hash<std::string_view>;
    using hash_type::operator();
    using is_transparent = void;
};
} // namespace astl
