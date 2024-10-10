// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <nlohmann/json.hpp>
#include <string_view>
#include <stdexcept>
#include <concepts>
#include <optional>
#include <cassert>
#include <string>

namespace ac::schema {

using Dict = nlohmann::json;
using OrderedDict = nlohmann::ordered_json;

class Object;

class SchemaItem {
    Object* m_parent = nullptr; // nullptr in root
    std::string_view m_key; // key in parent space, empty in root

    std::string_view m_desc; // optional human-readable description
    Dict m_defaultVal; // optional default value
    bool m_required = false; // is value required in parent
protected:
    // optionally materialized refernce to self in parent
    // for optional object it potentially be left null forever
    Dict* m_self = nullptr;

    ~SchemaItem() = default;

    // inline and defined below because they depend on Object
    void materializeSelf(); // materialize self in parent
    void tryAttachSelf(); // try to attach self to parent if possible (failing is not an error)

    // desc
    virtual void doDescribeSelf(OrderedDict& d) const = 0;

    void commonDescribeSelf(OrderedDict& d) const {
        if (!m_desc.empty()) {
            d["description"] = m_desc;
        }
        doDescribeSelf(d);
        if (!m_defaultVal.is_null()) {
            d["default"] = m_defaultVal;
        }
    }
public:
    SchemaItem() = default;
    SchemaItem(Dict& root) : m_self(&root) {} // attach as root
    SchemaItem(Object& parent, std::string_view key, std::string_view desc = {}, Dict def = {}, bool required = false)
        : m_parent(&parent)
        , m_key(key)
        , m_desc(desc)
        , m_defaultVal(std::move(def))
        , m_required(required)
    {
        tryAttachSelf();
    }

    std::string_view key() const {
        assert(m_parent); // should not be called on root items
        return m_key;
    }

    std::string_view description() const {
        return m_desc;
    }

    bool required() const {
        return m_required;
    }

    const Dict& defaultVal() const {
        return m_defaultVal;
    }

    void describeSelf(OrderedDict& d) const {
        if (m_parent) {
            auto& c = d[m_key];
            commonDescribeSelf(c);
        }
        else {
            commonDescribeSelf(d);
        }
    }
};

class Object : public SchemaItem {
    friend SchemaItem;
    std::vector<SchemaItem*> m_children;

    void doDescribeSelf(OrderedDict& d) const override {
        d["type"] = "object";
        auto& props = d["properties"];
        props = Dict::object(); // can't leave it null, even if it ends up empty

        auto req = OrderedDict::array();
        for (auto c : m_children) {
            c->describeSelf(props);
            if (c->required()) {
                req.push_back(c->key());
            }
        }

        if (!req.empty()) {
            d["required"] = std::move(req);
        }
    }
public:
    using SchemaItem::SchemaItem;
};

void SchemaItem::materializeSelf() {
    if (m_self) return; // already materialized
    if (!m_parent) {
        throw std::runtime_error("Cannot materialize self without parent");
    }
    m_parent->materializeSelf();
    auto& p = *m_parent->m_self;
    m_self = &p[m_key];
}

void SchemaItem::tryAttachSelf() {
    // add to parent list
    m_parent->m_children.push_back(this);

    // try to attach self to parent, but don't treat failure as error

    if (!m_parent->m_self) return; // can't attach with empty parent
    auto& p = *m_parent->m_self;

    // parent must be object or uninitialized promotable null
    assert(p.type() == Dict::value_t::object || p.type() == Dict::value_t::null);

    auto f = p.find(m_key);
    if (f == p.end()) return; // missing in parent, can't attach
    m_self = &*f;
}

template <std::derived_from<SchemaItem> T>
class Array : public SchemaItem {
    void doDescribeSelf(OrderedDict& d) const override {
        d["type"] = "array";
        T{}.describeSelf(d["items"]);
    }
public:
    using SchemaItem::SchemaItem;

    size_t size() const {
        if (!m_self) return 0;
        return m_self->size();
    }

    T operator[](size_t i) const {
        return T(m_self->at(i));
    }

    T emplace_back() {
        materializeSelf();
        return T(m_self->emplace_back());
    }
};

template <typename T>
class Primitive : public SchemaItem {
    static std::string_view ptn(bool) { return "boolean"; }
    static std::string_view ptn(int) { return "integer"; }
    static std::string_view ptn(unsigned) { return "integer"; }
    static std::string_view ptn(double) { return "number"; }
    static std::string_view ptn(float) { return "number"; }
    static std::string_view ptn(std::string) { return "string"; }
    static std::string_view ptn(std::string_view) { return "string"; }

    void doDescribeSelf(OrderedDict& d) const override {
        d["type"] = ptn(T());
    }
public:
    using SchemaItem::SchemaItem;

    std::optional<T> getValue() const {
        if (!m_self) {
            if (required()) {
                throw std::runtime_error("Required value is missing");
            }
            if (!defaultVal().is_null()) {
                return defaultVal().get<T>();
            }
            return std::nullopt;
        }
        return m_self->get<T>();
    }

    void setValue(T n) {
        materializeSelf();
        *m_self = n;
    }
};

using Bool = Primitive<bool>;
using String = Primitive<std::string_view>;
using Int = Primitive<int>;
using Uint = Primitive<unsigned>;
using Double = Primitive<double>;

} // namespace ac::schema
