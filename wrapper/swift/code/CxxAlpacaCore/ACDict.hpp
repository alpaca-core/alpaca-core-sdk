// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include <ac/Dict.hpp>

#include <swift/bridging>

#include "AlpacaCore-Swift.h"

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

std::string getDictTypeAsString(const SwiftACDict& dict);

std::shared_ptr<local::ModelDesc> getModelDesc();

swift::String getSwiftString();

}
