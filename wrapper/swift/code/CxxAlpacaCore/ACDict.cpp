// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ACDict.hpp"
#include <iostream>

namespace ac {

SwiftACDict::SwiftACDict()
    : m_dict(new Dict())
    , m_owned(true)
{}

SwiftACDict::SwiftACDict(const SwiftACDict& dict)
{
    if (dict.m_owned) {
        m_dict.reset(new Dict(*dict.m_dict));
    } else {
        m_dict.reset();
        m_dict.reset(dict.m_dict.get());
    }

    m_owned = dict.m_owned;
}

SwiftACDict& SwiftACDict::operator=(const SwiftACDict& dict) {
    if (this != &dict) {
        *m_dict = *dict.m_dict;
        m_owned = true;
    }

    return *this;
}

SwiftACDict::~SwiftACDict() {
    if (!m_owned) {
        m_dict.release();
    }
}

void SwiftACDict::parseJson(const char* json, unsigned length) {
    const char* json_end = json + length;
    *m_dict = ac::Dict::parse(json, json_end);
}

SwiftACDict SwiftACDict::getDictAt(KeyType key) const {
    auto f = m_dict->find(key);
    return makeCopy(*f);
}

bool SwiftACDict::getBool() const {
    return m_dict->get<bool>();
}

int SwiftACDict::getInt() const {
    return m_dict->get<int>();
}

double SwiftACDict::getDouble() const {
    return m_dict->get<double>();
}

std::string SwiftACDict::getString() const {
    return m_dict->get<std::string>();
}

std::vector<SwiftACDict> SwiftACDict::getArray() const {
    std::vector<SwiftACDict> vec;
    for (size_t i = 0; i < m_dict->size(); i++)
    {
        vec.push_back(makeCopy(m_dict->at(i)));
    }

    return vec;
}

Blob SwiftACDict::getBinary() const {
    return m_dict->get_binary();
}

std::string SwiftACDict::dump() const {
    return m_dict->dump();
}

DictValueType SwiftACDict::getType() const {
    switch (m_dict->type()) {
        case Dict::value_t::null: return DictValueType::DVT_Null;
        case Dict::value_t::boolean: return DictValueType::DVT_Bool;
        case Dict::value_t::number_integer: return DictValueType::DVT_Int;
        case Dict::value_t::number_unsigned: return DictValueType::DVT_Unsigned;
        case Dict::value_t::number_float: return DictValueType::DVT_Double;
        case Dict::value_t::string: return DictValueType::DVT_String;
        case Dict::value_t::array: return DictValueType::DVT_Array;
        case Dict::value_t::object: return DictValueType::DVT_Object;
        case Dict::value_t::binary: return DictValueType::DVT_Binary;
        default:
            return DictValueType::DVT_Null;
    }
}

SwiftACDict SwiftACDict::makeCopy(Dict& dict) const {
    SwiftACDict d;
    *d.m_dict = dict;
    d.m_owned = true;

    return d;
}

SwiftACDict SwiftACDict::makeRef(Dict& dictRef) const {
    SwiftACDict dict;
    dict.m_dict.reset(&dictRef);
    dict.m_owned = false;

    return dict;
}

std::string getDictTypeAsString(const SwiftACDict& dict) {
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

    return dictTypeStrings[static_cast<int>(dict.getType())];
}

}
