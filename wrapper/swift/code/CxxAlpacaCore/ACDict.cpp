// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ACDict.hpp"
#include <iostream>

namespace {

template <typename F>
auto dict_try_catch(F&& f) noexcept -> decltype(f()) {
    try {
        return f();
    }
    catch (const std::exception& e) {
        std::cout << e.what();
    }
    catch (...) {
        std::cout << "Unknown error" << std::endl;
    }
    return {};
}

}

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
    if (!m_dict->is_object()) {
        std::cerr << "Dict is not object!" << std::endl;
        return {};
    }

    auto f = m_dict->find(key);
    if (f == m_dict->end()) {
        return SwiftACDict();
    }
    return makeCopy(*f);
}

bool SwiftACDict::getBool() const {
    return dict_try_catch([&] {
        return m_dict->get<bool>();
    });
}

int SwiftACDict::getInt() const {
    return dict_try_catch([&] {
        return m_dict->get<int>();
    });
}

unsigned SwiftACDict::getUnsigned() const {
    return dict_try_catch([&] {
        return m_dict->get<unsigned>();
    });
}

double SwiftACDict::getDouble() const {
    return dict_try_catch([&] {
        return m_dict->get<double>();
    });
}

std::string SwiftACDict::getString() const {
    return dict_try_catch([&] {
        return m_dict->get<std::string>();
    });
}

std::vector<SwiftACDict> SwiftACDict::getArray() const {
    if (!m_dict->is_array()) {
        std::cerr << "Dict is not array!" << std::endl;
        return {};
    }

    std::vector<SwiftACDict> vec;
    for (size_t i = 0; i < m_dict->size(); i++)
    {
        vec.push_back(makeCopy(m_dict->at(i)));
    }

    return vec;
}

Blob SwiftACDict::getBinary() const {
    if (!m_dict->is_binary()) {
        std::cerr << "Dict is not binary!" << std::endl;
        return {};
    }

    return m_dict->get_binary();
}

void SwiftACDict::setDictAt(KeyType key, SwiftACDict value) {
    (*m_dict)[key] = *value.m_dict;
}

void SwiftACDict::setBool(bool value) {
    *m_dict = value;
}

void SwiftACDict::setInt(int value) {
    *m_dict = value;
}

void SwiftACDict::setUnsigned(unsigned value) {
    *m_dict = value;
}

void SwiftACDict::setDouble(double value) {
    *m_dict = value;
}

void SwiftACDict::setString(const std::string& value) {
    *m_dict = value;
}

void SwiftACDict::setArray(const std::vector<SwiftACDict>& value) {
    *m_dict = ac::Dict::array();
    for (const auto& v : value) {
        m_dict->push_back(*v.m_dict);
    }
}

void SwiftACDict::setBinary(const uint8_t* data, uint32_t size) {
    *m_dict = ac::Dict::binary(ac::Blob(data, data+size));
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

std::shared_ptr<local::ModelDesc> getModelDesc() {
    return std::make_shared<local::ModelDesc>();
}

swift::String getSwiftString(const swift::String& json) {
    auto cppStr = (std::string)json;
    AlpacaCore::SwiftStr s = AlpacaCore::SwiftStr::init();
    // s.parseJson(json);
    // auto str = s.dump();

    return s.dump();
}

}
