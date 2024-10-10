// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ACDict.hpp"
#include <iostream>

namespace ac {

DictRef::DictRef(Dict* _Nonnull root)
    : m_dictRef(root)
{}

DictRef DictRef::addChild(const std::string& key) const {
    Dict& child = (*m_dictRef)[((std::string)key)];
    return DictRef(&child);
}

DictRef DictRef::addElement() const {
    Dict& elem = (*m_dictRef).emplace_back(ac::Dict());
    return DictRef(&elem);
}

DictRef DictRef::atKey(const std::string& key) const {
    return DictRef(&m_dictRef->at((std::string)key));
}

DictRef DictRef::atIndex(int index) const {
    return DictRef(&m_dictRef->at(index));
}

swift::Int DictRef::getSize() const {
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

std::string DictRef::getString() const {
    return std::string(m_dictRef->get<std::string>());
}

BinaryBuffer DictRef::getBinary() const SWIFT_RETURNS_INDEPENDENT_VALUE {
    auto blob = m_dictRef->get_binary();
    return {blob.data(), swift::Int(blob.size())};
}

std::vector<std::string> DictRef::getKeys() const {
    std::vector<std::string> keys;
    for (auto it = m_dictRef->begin(); it != m_dictRef->end(); ++it) {
        keys.push_back(it.key());
    }
    return keys;
}

void DictRef::setBool(bool value) {
    *m_dictRef = value;
}

void DictRef::setInt(swift::Int value) {
    *m_dictRef = value;
}

void DictRef::setUnsigned(unsigned value) {
    *m_dictRef = value;
}

void DictRef::setDouble(double value) {
    *m_dictRef = value;
}

void DictRef::setString(std::string value) {
    *m_dictRef = value;
}

void DictRef::setBinary(const void* data, swift::Int size) {
    *m_dictRef = Dict::binary(ac::Blob((uint8_t*)data, (uint8_t*)data + size));
}

void DictRef::parse(const std::string& jsonStr) {
    *m_dictRef = Dict::parse((std::string)jsonStr);
}

std::string DictRef::dump() const {
    return m_dictRef->dump();
}

DictRef DictRef::operator[](const std::string& key) const{
    return atKey(key);
}

DictRef DictRef::operator[](int index) const {
    return atIndex(index);
}

DictRoot* _Nonnull DictRef::clone() {
    DictRoot* root = DictRoot::create();
    *root->getRef().m_dictRef = *m_dictRef;
    return root;
}

DictRoot* _Nonnull DictRoot::create() {
    return new DictRoot();
}

DictRoot* _Nonnull DictRoot::parse(const std::string& jsonStr) {
    auto root = new DictRoot();
    root->m_dict = Dict::parse(jsonStr);
    return root;
}

DictRef DictRoot::getRef(){
    return DictRef(&m_dict);
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

}

void retainDictRoot(ac::DictRoot* _Nullable d) {
    d->retain();
}

void releaseDictRoot(ac::DictRoot* _Nullable d) {
    d->release();
}
