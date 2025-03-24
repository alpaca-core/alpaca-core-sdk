// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <string>
#include <string_view>
#include <charconv>
#include <cstdint>

namespace astl {

// utils to give a human readable id to an object

inline std::string id_from_ptr(const void* ptr, std::string_view prefix = "0x") {
    char hex[18] = {};
    auto r = std::to_chars(hex, hex + sizeof(hex), reinterpret_cast<uintptr_t>(ptr), 16);
    std::string_view hexv(hex, r.ptr - hex);

    std::string ret;
    ret.reserve(prefix.size() + hexv.size());
    ret.append(prefix.data(), prefix.size());
    ret.append(hexv.data(), hexv.size());

    return ret;
}

inline std::string id_or_ptr(std::string_view id, const void* ptr, std::string_view prefix = "0x") {
    return id.empty() ? id_from_ptr(ptr, prefix) : std::string(id);
}

} // namespace astl
