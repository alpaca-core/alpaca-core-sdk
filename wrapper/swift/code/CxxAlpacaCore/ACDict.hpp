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

class DictRoot;

class DictRef {
public:
    DictRef(const DictRef& other) = default;

    DictRef atKey(const swift::String& key) const;
    DictRef atIndex(int index) const;

    unsigned getSize() const;
    DictValueType getType() const;

    bool getBool() const;
    int getInt() const;
    unsigned getUnsigned() const;
    double getDouble() const;
    swift::String getString() const;
    DictRef getArray(int index) const;
    DictRef getObject(const swift::String& key) const;
    std::vector<uint8_t>& getBinary() const;

    void parse(const swift::String& jsonStr);

private:
    friend class DictRoot;
    DictRef(Dict* _Nonnull root);

    Dict* _Nonnull m_dictRef;
};

class DictRoot : public IntrusiveRefCounted<DictRoot> {
public:
    DictRoot() = default;
    DictRoot(const DictRoot&) = delete;

    static DictRoot* _Nonnull create();
    // static DictRoot* _Nonnull parse(const std::string& jsonStr);
    // static DictRoot* _Nonnull parse();

    void parse(const swift::String& key);
    DictRef addChild(const swift::String& key);
    DictRef getDictRef(const swift::String& key = "");

private:
    Dict m_dict;
} SWIFT_SHARED_REFERENCE(retainDictRoot, releaseDictRoot);

std::string getDictTypeAsString(DictValueType type);
swift::String getSwiftString(const swift::String& json);

}

void retainDictRoot(ac::DictRoot* _Nullable d);
void releaseDictRoot(ac::DictRoot* _Nullable d);
