// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ACDict.hpp"
#include <iostream>

namespace ac {

DictRef::DictRef(Dict* _Nonnull root)
    : m_dictRef(root)
{}

DictRef DictRef::atKey(const swift::String& key) const {
    return DictRef(&m_dictRef->at((std::string)key));
}

DictRef DictRef::atIndex(int index) const {
    return DictRef(&m_dictRef->at(index));
}

unsigned DictRef::getSize() const {
    return m_dictRef->size();
}

DictValueType DictRef::getType() const {
    switch (m_dictRef->type()) {
        case ac::Dict::value_t::null: return DictValueType::DVT_Null;
        case ac::Dict::value_t::boolean: return DictValueType::DVT_Bool;
        case ac::Dict::value_t::number_integer: return DictValueType::DVT_Int;
        case ac::Dict::value_t::number_unsigned: return DictValueType::DVT_Unsigned;
        case ac::Dict::value_t::number_float: return DictValueType::DVT_Double;
        case ac::Dict::value_t::string: return DictValueType::DVT_String;
        case ac::Dict::value_t::array: return DictValueType::DVT_Array;
        case ac::Dict::value_t::object: return DictValueType::DVT_Object;
        case ac::Dict::value_t::binary: return DictValueType::DVT_Binary;
        default:
            throw std::runtime_error("Unsupported dict value type!");
            return DictValueType::DVT_Null;
    }
}

bool DictRef::getBool() const {
    return m_dictRef->get<bool>();
}

int DictRef::getInt() const {
    return m_dictRef->get<int>();
}

unsigned DictRef::getUnsigned() const {
    return m_dictRef->get<unsigned>();
}

double DictRef::getDouble() const {
    return m_dictRef->get<double>();
}

swift::String DictRef::getString() const {
    return swift::String(m_dictRef->get<std::string>());
}

DictRef DictRef::getArray(int index) const {
    // auto arr = m_dictRef->get_array();
    // return DictRef(&m_dictRef->at(index));
    return *this;
}

DictRef DictRef::getObject(const swift::String& key) const {
    return *this;
}

std::vector<uint8_t>& DictRef::getBinary() const {
    return m_dictRef->get_binary();
}

void DictRef::parse(const swift::String& jsonStr) {
    *m_dictRef = Dict::parse((std::string)jsonStr);
}

DictRoot* _Nonnull DictRoot::create() {
    return new DictRoot();
}

// DictRoot* _Nonnull DictRoot::parse(const std::string& jsonStr) {
//     auto root = new DictRoot();
//     root->parse(jsonStr);
//     return root;
// }

// DictRoot* _Nonnull DictRoot::parse() {
//     return new DictRoot();
// }

void DictRoot::parse(const swift::String& key) {
    m_dict = Dict::parse((std::string)key);
}

DictRef DictRoot::addChild(const swift::String& key) {
    Dict& child = m_dict[((std::string)key)];
    return DictRef(&child);
}

DictRef DictRoot::getDictRef(const swift::String& key){
    std::string keyStr = (std::string)key;
    if (keyStr.empty()) {
        return DictRef(&m_dict);
    }
    return DictRef(&m_dict[keyStr]);
}

std::string getDictTypeAsString(DictValueType type) {
    static const char* dictTypeStrings[] = {
        "Null",
        "Bool",
        "Int",
        "Unsigned",
        "Double",
        "String",
        "Array",
        "Object",
        "Binary"
    };

    return dictTypeStrings[static_cast<int>(type)];
}

swift::String getSwiftString(const swift::String& json)  {
    auto cppStr = (std::string)json;
    return swift::String("pesho e ovca");
}

}

void retainDictRoot(ac::DictRoot* _Nullable d) {
    d->retain();
}

void releaseDictRoot(ac::DictRoot* _Nullable d) {
    d->release();
}
