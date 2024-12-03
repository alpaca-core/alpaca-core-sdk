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

    template <std::signed_integral I>
    static void describeField(Dict& obj) {
        obj["type"] = "integer";
    }

    template <std::unsigned_integral I>
    static void describeField(Dict& obj) {
        obj["type"] = "integer";
    }

    template <std::floating_point F>
    static void describeField(Dict& obj) {
        obj["type"] = "number";
    }

    template <std::same_as<std::string> S>
    static void describeField(Dict& obj) {
        obj["type"] = "string";
    }

    template <std::same_as<bool> B>
    static void describeField(Dict& obj) {
        obj["type"] = "boolean";
    }

    template <std::same_as<std::nullptr_t> N>
    static void describeField(Dict& obj) {
        obj["type"] = "null";
    }

    template <Visitable V>
    static void describeField(Dict& d) {
        SchemaVisitor v(d);
        V schema;
        schema.visitFields(v);
    }

    template <typename Vec>
    static void describeField(Dict& obj) {
        obj["type"] = "array";
        describeField<typename Vec::value_type>(obj["items"]);
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

