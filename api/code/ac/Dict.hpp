// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <nlohmann/json.hpp>

namespace ac {
using Dict = nlohmann::json;
using Blob = std::vector<uint8_t>;

template <typename T>
T Dict_optValueAt(Dict& dict, std::string_view key, T defaultValue) {
    auto f = dict.find(key);
    if (f == dict.end()) return defaultValue;
    return f->get<T>();
}

template <typename T>
void Dict_optApplyValueAt(const Dict& dict, std::string_view key, T& value) {
    auto f = dict.find(key);
    if (f == dict.end()) return;
    value = f->get<T>();
}

} // namespace ac
