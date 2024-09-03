// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <nlohmann/json.hpp>

namespace ac {

/**
 * @brief A type alias for nlohmann::json, representing a dictionary-like structure.
 */
using Dict = nlohmann::json;

/**
 * @brief A type alias for a vector of bytes, representing binary data.
 */
using Blob = std::vector<uint8_t>;

/**
 * @brief Retrieves an optional value from a Dict object.
 * 
 * This function attempts to find a value in the Dict object using the provided key.
 * If the key is not found, it returns the default value.
 * 
 * @tparam T The type of the value to retrieve and return.
 * @param dict The Dict object to search in.
 * @param key The key to look for in the Dict.
 * @param defaultValue The value to return if the key is not found.
 * @return The value associated with the key if found, otherwise the default value.
 */
template <typename T>
T Dict_optValueAt(Dict& dict, std::string_view key, T defaultValue) {
    auto f = dict.find(key);
    if (f == dict.end()) return defaultValue;
    return f->get<T>();
}

/**
 * @brief Applies a value from a Dict object to a given variable if the key exists.
 * 
 * This function attempts to find a value in the Dict object using the provided key.
 * If the key is found, it applies the value to the provided variable.
 * If the key is not found, the variable remains unchanged.
 * 
 * @tparam T The type of the value to retrieve and apply.
 * @param dict The Dict object to search in.
 * @param key The key to look for in the Dict.
 * @param value The variable to which the value will be applied if found.
 */
template <typename T>
void Dict_optApplyValueAt(const Dict& dict, std::string_view key, T& value) {
    auto f = dict.find(key);
    if (f == dict.end()) return;
    value = f->get<T>();
}

} // namespace ac
