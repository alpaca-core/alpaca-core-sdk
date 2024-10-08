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

class SwiftACDict {
public:

    SwiftACDict();
    SwiftACDict(const SwiftACDict& dict);
    SwiftACDict& operator=(const SwiftACDict& dict);

    ~SwiftACDict();

    void parseJson(const char* json, unsigned length);

    using KeyType = std::string;

    // Getters
    SwiftACDict getDictAt(KeyType key) const;
    bool getBool() const;
    int getInt() const;
    unsigned getUnsigned() const;
    double getDouble() const;
    std::string getString() const;
    std::vector<SwiftACDict> getArray() const;
    Blob getBinary() const;

    // Setters
    void setDictAt(KeyType key, SwiftACDict value);
    void setBool(bool value);
    void setInt(int value);
    void setUnsigned(unsigned value);
    void setDouble(double value);
    void setString(const std::string& value);
    void setArray(const std::vector<SwiftACDict>& value);
    void setBinary(const uint8_t* data, uint32_t size);

    std::string dump() const;
    DictValueType getType() const;

private:
    SwiftACDict makeCopy(Dict& dict) const;
    SwiftACDict makeRef(Dict& dictRef) const;

    std::unique_ptr<Dict> m_dict;
    bool m_owned;
};


class DictRoot : public IntrusiveRefCounted<DictRoot> {
public:
    DictRoot() = default;
    DictRoot(const swift::String& jsonStr);
    DictRoot(const DictRoot&) = delete;

    static DictRoot* _Nonnull create();

private:
    Dict m_dict;
} SWIFT_SHARED_REFERENCE(retainDictRoot, releaseDictRoot);

void retainDictRoot(DictRoot* _Nullable);
void releaseDictRoot(DictRoot* _Nullable);

class DictRef : public IntrusiveRefCounted<DictRoot> {
public:
    DictRef() = delete;
    DictRef(DictRoot* _Nonnull dict);
    DictRef(const DictRef&) = delete;

    static DictRef* _Nonnull create(DictRoot* _Nonnull dictRoot);

private:
    Dict* m_dictRef;
} SWIFT_SHARED_REFERENCE(retainDictRef, releaseDictRoot);

void retainDictRef(DictRef* _Nullable);
void releaseDictRef(DictRef* _Nullable);

std::string getDictTypeAsString(const SwiftACDict& dict);

// swift::String getSwiftString();

}
