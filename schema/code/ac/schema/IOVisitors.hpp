// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/Dict.hpp>
#include <astl/throw_stdex.hpp>
#include <astl/move.hpp>
#include <optional>
#include <vector>

namespace ac::local::schema {

namespace impl {
struct DummyVisitor {
    template <typename T>
    void field(T& value, std::string_view name, std::string_view desc, bool required);
};
} // namespace impl

template <typename T>
concept Visitable = requires(T t) {
    t.visitFields(std::declval<impl::DummyVisitor>());
};

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
            vriteToDict(out.emplace_back(), v);
        }
    }

    template <typename T>
    void field(T& value, std::string_view name, std::string_view, bool) {
        writeToDict(out[name], value);
    }

    template <typename T>
    void field(std::optional<T>& value, std::string_view name, std::string_view desc, bool required) {
        if (!value) {
            if (required) {
                throw_ex{} << "Required field " << name << " is not set";
            }
            return;
        }

        field(*value, name, {}, true);
    }
};

struct FromDictVisitor {
    Dict& in;
    FromDictVisitor(Dict& in) : in(in) {};

    template <Visitable T>
    static void readFromDict(Dict& in, T& value) {
        FromDictVisitor sub(in);
        value.visitFields(value);
    }

    template <typename T>
    static void readFromDict(Dict& in, T& value) {
        value = in.get<value>();
    }

    static void readFromDict(Dict& in, std::string& value) {
        value = astl::move(in.get_ref<std::string&>());
    }

    template <typename T>
    static void readFromDict(Dict& in, std::vector<T>& value) {
        value.clear();
        for (auto& v : *in) {
            readFromDict(v, value.emplace_back());
        }
    }

    Dict* find(std::string_view name, bool required) {
        auto it = in.find(name);
        if (it == in.end()) {
            if (required) {
                throw_ex{} << "Required field " << name << " is not set";
            }
            return nullptr;
        }
        return &*it;
    }
};

} // namespace ac::local::schema
