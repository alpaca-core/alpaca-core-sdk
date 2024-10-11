// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "SchemaItem.hpp"
#include <magic_enum.hpp>

namespace ac::schema {

template <typename EnumType>
class Enum : public String {
    static_assert(std::is_enum_v<EnumType>);

    void doDescribeSelf(OrderedDict& d) const override {
        String::doDescribeSelf(d);
        d["enum"] = magic_enum::enum_names<EnumType>();
    }
public:
    using String::String;

    std::optional<EnumType> optGetValue() const {
        auto strVal = String::optGetValue();
        if (!strVal) return {};

        auto opt = magic_enum::enum_cast<EnumType>(*strVal);
        if (!opt) {
            throw std::invalid_argument("Invalid enum value");
        }
        return *opt;
    }

    EnumType getValue() const {
        auto opt = optGetValue();
        if (!opt) {
            throw std::runtime_error("Value is missing");
        }
        return *opt;
    }

    void setValue(EnumType value) {
        String::setValue(magic_enum::enum_name(value));
    }

    void setValue(std::string_view value) {
        auto opt = magic_enum::enum_cast<EnumType>(value);
        if (!opt.has_value()) {
            throw std::invalid_argument("Invalid enum value");
        }
        String::setValue(value);
    }
};

} // namespace ac::schema
