// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "IOVisitors.hpp"
#include <concepts>

namespace ac::local::schema {

template <typename Dict = ac::Dict>
struct SchemaVisitor {
    Dict& out;
    Dict* props;
    std::vector<std::string_view> req;

    ~SchemaVisitor() {
        if (!req.empty()) {
            out["required"] = req;
        }
    }

    SchemaVisitor(Dict& out) : out(out) {
        out["type"] = "object";
        props = &out["properties"];
    }

    template <typename T>
    static void describeField(Dict& obj) {
        if constexpr (std::signed_integral<T> || std::unsigned_integral<T>) {
            obj["type"] = "integer";
        }
        else if constexpr (std::floating_point<T>) {
            obj["type"] = "number";
        }
        else if constexpr (std::same_as<T, std::string> || std::same_as<T, std::string_view>) {
            obj["type"] = "string";
        }
        else if constexpr (std::same_as<T, bool>) {
            obj["type"] = "boolean";
        }
        else if constexpr (std::same_as<T, std::nullptr_t>) {
            obj["type"] = "null";
        }
        else if constexpr (Visitable<T>) {
            SchemaVisitor v(obj);
            T schema;
            schema.visitFields(v);
        }
        else {
            // assume array
            obj["type"] = "array";
            describeField<typename T::value_type>(obj["items"]);
        }
    }

    template <typename T>
    void operator()(Field<T>& t, std::string_view name, std::string_view desc) {
        auto& obj = (*props)[name];
        describeField<T>(obj);

        if (!desc.empty()) {
            obj["description"] = desc;
        }
        if (t.defaultSet()) {
            ToDictVisitor<Dict>::writeToDict(obj["default"], t.value());
        }
        else if (t.required()) {
            req.push_back(name);
        }
    }
};

template <typename T, typename Dict>
void Struct_toSchema(Dict& out) {
    SchemaVisitor<Dict>::describeField<T>(out);
}

} // namespace ac::local::schema

