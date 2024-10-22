// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <nlohmann/json.hpp>

namespace ac {

/// @defgroup cpp-dict ac::Dict
/// Dictionary type and utils.
/// @{

/// Dictionary type. A type alias for [nlohmann::json](https://github.com/nlohmann/json).
using Dict = nlohmann::json;

/// Blob (binary) carrier type.
using Blob = std::vector<uint8_t>;

/// Utility function to return an optional value from a Dict object at a given key.
template <typename T>
T Dict_optValueAt(const Dict& dict, std::string_view key, T defaultValue) {
    auto f = dict.find(key);
    if (f == dict.end()) return defaultValue;
    return f->get<T>();
}

/// Utility function to get a value from a Dict object at a given key.
/// If the key is found, it sets the value to the output parameter and returns true.
/// If the key is not found, it returns false.
template <typename T>
bool Dict_optApplyValueAt(const Dict& dict, std::string_view key, T& value) {
    auto f = dict.find(key);
    if (f == dict.end()) return false;
    value = f->get<T>();
    return true;
}

/// @}

} // namespace ac
