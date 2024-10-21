// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include <ac/Dict.hpp>

#include <swift/bridging>
#include "AlpacaCore-Swift.h"

#include "IntrusiveRefCounted.hpp"

namespace ac::swift {

namespace sw = ::swift;

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
    DictRoot* _Nonnull clone();

    DictRef operator[](const std::string& key) const;
    DictRef operator[](int index) const;

    Dict& getDict() const { return *m_dictRef; }


private:
    friend class DictRoot;
    friend class Model;
    friend class Instance;
    DictRef(Dict* _Nonnull root);

    Dict* _Nonnull m_dictRef;
};

class DictRoot : public IntrusiveRefCounted<DictRoot> {
public:
    DictRoot() = default;
    DictRoot(const DictRoot&) = delete;

    static DictRoot* _Nonnull create();
    static DictRoot* _Nonnull parse(const std::string& jsonStr);

    DictRef getRef();

private:
    Dict m_dict;
} SWIFT_SHARED_REFERENCE(retainDictRoot, releaseDictRoot);

std::string getDictTypeAsString(DictValueType type);

}

void retainDictRoot(ac::swift::DictRoot* _Nullable d);
void releaseDictRoot(ac::swift::DictRoot* _Nullable d);
