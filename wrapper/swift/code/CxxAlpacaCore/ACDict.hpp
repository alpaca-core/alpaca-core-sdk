// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include <ac/Dict.hpp>

#include <swift/bridging>
#include "AlpacaCore-Swift.h"

#include "IntrusiveRefCounted.hpp"

#include <memory>
#include <vector>

namespace ac{

enum class DictValueType {
    DVT_Null,         /**< Null value */
    DVT_Bool,         /**< Boolean value */
    DVT_Int,          /**< Integer number */
    DVT_Unsigned,     /**< Unsigned integer number */
    DVT_Double,       /**< Floating-point number */
    DVT_String,       /**< String value */
    DVT_Array,        /**< Array of values */
    DVT_Object,       /**< Object (key-value pairs) */
    DVT_Binary,       /**< Binary data */
};

struct BinaryBuffer {
    const uint8_t* _Nonnull data;
    swift::Int size;
};

class DictRoot;

class DictRef {
public:
    DictRef(const DictRef& other) = default;

    DictRef addChild(const std::string& key) const;
    DictRef addElement() const;

    DictRef atKey(const std::string& key) const;
    DictRef atIndex(int index) const;

    swift::Int getSize() const;
    DictValueType getType() const;

    bool getBool() const;
    int getInt() const;
    unsigned getUnsigned() const;
    double getDouble() const;
    std::string getString() const;
    BinaryBuffer getBinary() const SWIFT_RETURNS_INDEPENDENT_VALUE;
    std::vector<std::string> getKeys() const;

    void setBool(bool value);
    void setInt(swift::Int value);
    void setUnsigned(unsigned value);
    void setDouble(double value);
    void setString(std::string value);
    void setBinary(const void* _Nonnull data, swift::Int size);

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

void retainDictRoot(ac::DictRoot* _Nullable d);
void releaseDictRoot(ac::DictRoot* _Nullable d);
