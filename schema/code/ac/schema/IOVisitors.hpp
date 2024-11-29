// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Field.hpp"
#include <ac/Dict.hpp>
#include <astl/throw_stdex.hpp>
#include <astl/move.hpp>
#include <optional>
#include <vector>

namespace ac::local::schema {

namespace impl {
struct DummyVisitor {
    template <typename T>
    void operator()(Field<T>& field, std::string_view name, std::string_view desc);
};
} // namespace impl

template <typename T>
concept Visitable = requires(T t) {
    t.visitFields(std::declval<impl::DummyVisitor&>());
};

template <typename Dict = ac::Dict>
struct ToDictVisitor {
    Dict& out;
    ToDictVisitor(Dict& out) : out(out) {}

    template <Visitable T>
    static void writeToDict(Dict& out, T& value) {
        ToDictVisitor sub(out);
        value.visitFields(sub);
    }

    template <typename T>
    static void writeToDict(Dict& out, T& value) {
        out = astl::move(value);
    }

    template <typename T>
    static void writeToDict(Dict& out, std::vector<T>& value) {
        out = Dict::array();
        for (auto& v : value) {
            writeToDict(out.emplace_back(), v);
        }
    }

    template <typename T>
    void operator()(Field<T>& field, std::string_view name, std::string_view) {
        if (field.defaultSet()) return; // don't be redundant
        if (!field.hasValue()) {
            if (field.required()) {
                throw_ex{} << "Required field " << name << " is not set";
            }
            // nothing to write for nullopt
            return;
        }
        writeToDict(out[name], field.value());
    }
};

template <typename T, typename Dict = ac::Dict>
Dict Struct_toDict(T&& s) {
    Dict ret;
    ToDictVisitor v(ret);
    s.visitFields(v);
    return ret;
}

struct FromDictVisitor {
    Dict& in;
    FromDictVisitor(Dict& in) : in(in) {};

    template <Visitable T>
    static void readFromDict(Dict& in, T& value) {
        FromDictVisitor sub(in);
        value.visitFields(sub);
    }

    template <typename T>
    static void readFromDict(Dict& in, T& value) {
        value = in.get<T>();
    }

    static void readFromDict(Dict& in, std::string& value) {
        value = astl::move(in.get_ref<std::string&>());
    }

    template <typename T>
    static void readFromDict(Dict& in, std::vector<T>& value) {
        value.clear();
        for (auto& v : in) {
            readFromDict(v, value.emplace_back());
        }
    }

    template <typename T>
    void operator()(Field<T>& field, std::string_view name, std::string_view) {
        auto it = in.find(name);
        if (it == in.end()) {
            if (field.required()) {
                throw_ex{} << "Required field " << name << " is not set";
            }
            // otherwise leave the field as is
            return;
        }

        readFromDict(*it, field.materialize());
    }
};

template <typename T>
T Struct_fromDict(Dict&& d) {
    T ret;
    FromDictVisitor v(d);
    ret.visitFields(v);
    return ret;
}

} // namespace ac::local::schema
