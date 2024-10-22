// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Dict.hpp"

namespace AC {

DictIterator::DictIterator(Dict& dict)
    : m_it(dict.begin())
    , m_endIt(dict.end())
{}

bool DictIterator::hasNext() const {
    return m_it != m_endIt;
}

std::string DictIterator::getKey() const {
    return m_it.key();
}

DictRef DictIterator::getValue() const {
    return DictRef(*m_it);
}

DictRef DictIterator::next() {
    return DictRef(*(m_it++));
}

DictRef::DictRef(Dict& root)
    : m_dictRef(root)
{}

DictRef DictRef::addChild(const std::string& key) const {
    Dict& child = m_dictRef[((std::string)key)];
    return DictRef(child);
}

DictRef DictRef::addElement() const {
    Dict& elem = m_dictRef.emplace_back(ac::Dict());
    return DictRef(elem);
}

DictRef DictRef::atKey(const std::string& key) const {
    return DictRef(m_dictRef.at((std::string)key));
}

DictRef DictRef::atIndex(int index) const {
    return DictRef(m_dictRef.at(index));
}

sw::Int DictRef::getSize() const {
    return m_dictRef.size();
}

DictValueType DictRef::getType() const {
    switch (m_dictRef.type()) {
        case ac::Dict::value_t::null: return DictValueType::Null;
        case ac::Dict::value_t::boolean: return DictValueType::Bool;
        case ac::Dict::value_t::number_integer: return DictValueType::Int;
        case ac::Dict::value_t::number_unsigned: return DictValueType::Unsigned;
        case ac::Dict::value_t::number_float: return DictValueType::Double;
        case ac::Dict::value_t::string: return DictValueType::String;
        case ac::Dict::value_t::array: return DictValueType::Array;
        case ac::Dict::value_t::object: return DictValueType::Object;
        case ac::Dict::value_t::binary: return DictValueType::Binary;
        default:
            assert(!"Unreachable");
            return DictValueType::Null;
    }
}

bool DictRef::getBool() const {
    return m_dictRef.get<bool>();
}

sw::Int DictRef::getInt() const {
    return m_dictRef.get<int>();
}

sw::UInt DictRef::getUnsigned() const {
    return m_dictRef.get<unsigned>();
}

double DictRef::getDouble() const {
    return m_dictRef.get<double>();
}

std::string DictRef::getString() const {
    return std::string(m_dictRef.get<std::string>());
}

BinaryBuffer DictRef::getBinary() const SWIFT_RETURNS_INDEPENDENT_VALUE {
    auto& blob = m_dictRef.get_binary();
    return {blob.data(), sw::Int(blob.size())};
}

std::vector<std::string> DictRef::getKeys() const {
    std::vector<std::string> keys;
    for (auto it = m_dictRef.begin(); it != m_dictRef.end(); ++it) {
        keys.push_back(it.key());
    }
    return keys;
}

void DictRef::setBool(bool value) {
    m_dictRef = value;
}

void DictRef::setInt(sw::Int value) {
    m_dictRef = value;
}

void DictRef::setUnsigned(sw::UInt value) {
    m_dictRef = value;
}

void DictRef::setDouble(double value) {
    m_dictRef = value;
}

void DictRef::setString(std::string value) {
    m_dictRef = value;
}

void DictRef::setBinary(const void* data, sw::Int size) {
    m_dictRef = Dict::binary(ac::Blob((uint8_t*)data, (uint8_t*)data + size));
}

void DictRef::parse(const std::string& jsonStr) {
    m_dictRef = Dict::parse((std::string)jsonStr);
}

std::string DictRef::dump() const {
    return m_dictRef.dump();
}

DictRef DictRef::operator[](const std::string& key) const{
    return atKey(key);
}

DictRef DictRef::operator[](int index) const {
    return atIndex(index);
}

DictRoot DictRef::clone() {
    DictRoot root;
    root.getRef().m_dictRef = m_dictRef;
    return root;
}

DictRoot DictRoot::parse(const std::string& jsonStr) {
    DictRoot root;
    *root.m_dict = Dict::parse(jsonStr);
    return root;
}

DictRef DictRoot::getRef() const {
    return DictRef(*m_dict);
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
