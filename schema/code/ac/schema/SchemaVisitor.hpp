// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "IOVisitors.hpp"
#include <concepts>

namespace ac::local::schema {

using OrderedDict = acnl::ordered_json;

struct SchemaVisitor {
    OrderedDict& out;
    OrderedDict* props;
    std::vector<std::string_view> req;

    ~SchemaVisitor() {
        if (!req.empty()) {
            out["required"] = req;
        }
    }

    SchemaVisitor(OrderedDict& out) : out(out) {
        out["type"] = "object";
        props = &out["properties"];
    }

    template <std::signed_integral I>
    static void describeField(OrderedDict& obj) {
        obj["type"] = "integer";
    }

    template <std::unsigned_integral I>
    static void describeField(OrderedDict& obj) {
        obj["type"] = "integer";
    }

    template <std::floating_point F>
    static void describeField(OrderedDict& obj) {
        obj["type"] = "number";
    }

    template <std::same_as<std::string> S>
    static void describeField(OrderedDict& obj) {
        obj["type"] = "string";
    }

    template <std::same_as<bool> B>
    static void describeField(OrderedDict& obj) {
        obj["type"] = "boolean";
    }

    template <std::same_as<std::nullptr_t> N>
    static void describeField(OrderedDict& obj) {
        obj["type"] = "null";
    }

    template <Visitable V>
    static void describeField(OrderedDict& d) {
        SchemaVisitor v(d);
        V schema;
        schema.visitFields(v);
    }

    template <typename Vec>
    static void describeField(OrderedDict& obj) {
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
            ToDictVisitor<OrderedDict>::writeToDict(obj["default"], t.value());
        }
        else if (t.required()) {
            req.push_back(name);
        }
    }
};

template <typename T>
void Struct_toSchema(OrderedDict& out) {
    SchemaVisitor::describeField<T>(out);
}

} // namespace ac::local::schema

