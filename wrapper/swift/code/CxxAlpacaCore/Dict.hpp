// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include <ac/Dict.hpp>

#include <swift/bridging>
#include "AlpacaCore-Swift.h"

namespace AC {

namespace sw = ::swift;
using namespace ac;

enum class DictValueType {
    Null,         /**< Null value */
    Bool,         /**< Boolean value */
    Int,          /**< Integer number */
    Unsigned,     /**< Unsigned integer number */
    Double,       /**< Floating-point number */
    String,       /**< String value */
    Array,        /**< Array of values */
    Object,       /**< Object (key-value pairs) */
    Binary,       /**< Binary data */
};

struct BinaryBuffer {
    const uint8_t* _Nonnull data;
    sw::Int size;
};

class DictRoot;
class DictRef;

class DictIterator {
public:
    DictIterator(const DictIterator& other) = default;

    bool hasNext() const;

    std::string getKey() const;
    DictRef getValue() const;
    DictRef next();
private:
    friend class DictRef;
    DictIterator(Dict& dict);

    ac::Dict::iterator m_it;
    ac::Dict::iterator m_endIt;
};

class DictRef {
public:
    DictRef(const DictRef& other) = default;

    DictRef addChild(const std::string& key) const;
    DictRef addElement() const;

    DictRef atKey(const std::string& key) const;
    DictRef atIndex(int index) const;

    sw::Int getSize() const;
    DictValueType getType() const;

    bool getBool() const;
    sw::Int getInt() const;
    sw::UInt getUnsigned() const;
    double getDouble() const;
    std::string getString() const;
    BinaryBuffer getBinary() const SWIFT_RETURNS_INDEPENDENT_VALUE;
    std::vector<std::string> getKeys() const;

    void setBool(bool value);
    void setInt(sw::Int value);
    void setUnsigned(sw::UInt value);
    void setDouble(double value);
    void setString(std::string value);
    void setBinary(const void* _Nonnull data, sw::Int size);

    void parse(const std::string& jsonStr);
    std::string dump() const;
    DictRoot clone();

    DictRef operator[](const std::string& key) const;
    DictRef operator[](int index) const;

    Dict& getDict() const { return m_dictRef; }
    DictIterator getIterator() const { return DictIterator(m_dictRef); }

private:
    friend class DictRoot;
    friend class Model;
    friend class Instance;
    friend class DictIterator;
    DictRef(Dict& root);

    Dict& m_dictRef;
};

class DictRoot {
public:
    DictRoot() {
        m_dict = std::make_shared<Dict>();
    }
    DictRoot(const DictRoot& other) {
        m_dict = other.m_dict;
    };

    static DictRoot parse(const std::string& jsonStr);

    DictRef getRef() const;

private:
    std::shared_ptr<Dict> m_dict;
};

std::string getDictTypeAsString(DictValueType type);

}
