// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "dict.h"
#include "DictCUtil.hpp"

#include <astl/move.hpp>
#include <splat/warnings.h>

#include <cstring>
#include <cassert>

DISABLE_MSVC_WARNING(4996) // posix name deprecated

struct ac_dict_root {
    ac::Dict dict;
};

struct ac_dict_iter {
    ac::Dict::iterator it;
    ac::Dict::iterator end;
};

namespace ac::cutil {
Dict Dict_from_dict_root_consume(ac_dict_root* d) {
    if (!d) return {};
    auto ret = std::move(d->dict);
    ac_dict_free_root(d);
    return ret;
}
ac_dict_root* Dict_to_dict_root(Dict d) {
    return new ac_dict_root{std::move(d)};
}
ac::Dict& Dict_from_dict_ref(ac_dict_ref ref) {
    assert(ref); // should never be null here
    return *reinterpret_cast<ac::Dict*>(ref);
}
ac_dict_ref Dict_to_dict_ref(ac::Dict& dict) {
    return reinterpret_cast<ac_dict_ref>(&dict);
}
ac::Dict Dict_parse(const char* json, const char* json_end) {
    if (!json_end) {
        json_end = json + strlen(json);
    }
    return ac::Dict::parse(json, json_end);
}
} // namespace ac::cutil

using namespace ac::cutil;

namespace {

thread_local std::string dict_last_error;

template <typename F>
auto dict_try_catch(F&& f) noexcept -> decltype(f()) {
    dict_last_error.clear();
    try {
        return f();
    }
    catch (const std::exception& e) {
        dict_last_error = e.what();
    }
    catch (...) {
        dict_last_error = "Unknown error";
    }
    return {};
}
} // namespace

const char* ac_dict_get_last_error() {
    if (dict_last_error.empty()) return nullptr;
    return dict_last_error.c_str();
}

extern "C" {
ac_dict_root* ac_dict_new_root() {
    return dict_try_catch([] {
        return new ac_dict_root();
    });
}

void ac_dict_free_root(ac_dict_root* d) {
    delete d;
}

bool ac_dict_parse_json(ac_dict_ref target, const char* json, const char* json_end) {
    return dict_try_catch([&] {
        Dict_from_dict_ref(target) = Dict_parse(json, json_end);
        return true;
    });
}

void ac_dict_copy(ac_dict_ref target, ac_dict_ref source) {
    Dict_from_dict_ref(target) = Dict_from_dict_ref(source);
}

void ac_dict_take(ac_dict_ref target, ac_dict_ref source) {
    Dict_from_dict_ref(target) = astl::move(Dict_from_dict_ref(source));
}

ac_dict_ref ac_dict_make_ref(ac_dict_root* d) {
    return Dict_to_dict_ref(d->dict);
}

ac_dict_ref ac_dict_at_key(ac_dict_ref d, const char* key) {
    return dict_try_catch([&] {
        return Dict_to_dict_ref(Dict_from_dict_ref(d).at(key));
    });
}

ac_dict_ref ac_dict_at_index(ac_dict_ref d, int index) {
    return dict_try_catch([&] {
        return Dict_to_dict_ref(Dict_from_dict_ref(d).at(index));
    });
}

ac_dict_value_type ac_dict_get_type(ac_dict_ref d) {
    switch (Dict_from_dict_ref(d).type()) {
        case ac::Dict::value_t::null: return ac_dict_value_type_null;
        case ac::Dict::value_t::boolean: return ac_dict_value_type_bool;
        case ac::Dict::value_t::number_integer: return ac_dict_value_type_number_int;
        case ac::Dict::value_t::number_unsigned: return ac_dict_value_type_number_unsigned;
        case ac::Dict::value_t::number_float: return ac_dict_value_type_number_double;
        case ac::Dict::value_t::string: return ac_dict_value_type_string;
        case ac::Dict::value_t::array: return ac_dict_value_type_array;
        case ac::Dict::value_t::object: return ac_dict_value_type_object;
        case ac::Dict::value_t::binary: return ac_dict_value_type_binary;
        default:
            dict_last_error = "Unsupported type";
            return ac_dict_value_type_null;
    }
}

int ac_dict_get_size(ac_dict_ref d) {
    return int(Dict_from_dict_ref(d).size());
}

bool ac_dict_get_bool_value(ac_dict_ref d) {
    return dict_try_catch([&] {
        return Dict_from_dict_ref(d).get<bool>();
    });
}

int ac_dict_get_int_value(ac_dict_ref d) {
    return dict_try_catch([&] {
        return Dict_from_dict_ref(d).get<int>();
    });
}

unsigned ac_dict_get_unsigned_value(ac_dict_ref d) {
    return dict_try_catch([&] {
        return Dict_from_dict_ref(d).get<unsigned>();
    });
}

double ac_dict_get_double_value(ac_dict_ref d) {
    return dict_try_catch([&] {
        return Dict_from_dict_ref(d).get<double>();
    });
}

const char* ac_dict_get_string_value(ac_dict_ref d) {
    return dict_try_catch([&] {
        return Dict_from_dict_ref(d).get<std::string_view>().data();
    });
}

ac_dict_binary_buf ac_dict_get_binary_value(ac_dict_ref d) {
    return dict_try_catch([&] {
        auto& bin = Dict_from_dict_ref(d).get_binary();
        return ac_dict_binary_buf{ bin.data(), uint32_t(bin.size()) };
    });
}

ac_dict_iter* ac_dict_new_iter(ac_dict_ref d) {
    return new ac_dict_iter{ Dict_from_dict_ref(d).begin(), Dict_from_dict_ref(d).end() };
}

ac_dict_iter* ac_dict_iter_next(ac_dict_iter* it) {
    ++it->it;
    if (it->it == it->end) {
        delete it;
        return nullptr;
    }
    return it;
}

void ac_dict_free_iter(ac_dict_iter* it) {
    delete it;
}

const char* ac_dict_iter_get_key(ac_dict_iter* it) {
    return dict_try_catch([&] {
        return it->it.key().data();
    });
}

ac_dict_ref ac_dict_iter_get_value(ac_dict_iter* it) {
    return Dict_to_dict_ref(it->it.value());
}

void ac_dict_set_null(ac_dict_ref parent) {
    Dict_from_dict_ref(parent) = nullptr;
}

void ac_dict_set_bool(ac_dict_ref parent, bool value) {
    Dict_from_dict_ref(parent) = value;
}

void ac_dict_set_int(ac_dict_ref parent, int value) {
    Dict_from_dict_ref(parent) = value;
}

void ac_dict_set_unsigned(ac_dict_ref parent, unsigned value) {
    Dict_from_dict_ref(parent) = value;
}

void ac_dict_set_double(ac_dict_ref parent, double value) {
    Dict_from_dict_ref(parent) = value;
}

void ac_dict_set_string(ac_dict_ref parent, const char* value, const char* end) {
    if (!end) {
        end = value + strlen(value);
    }
    Dict_from_dict_ref(parent) = std::string(value, end);
}

void ac_dict_set_array(ac_dict_ref parent) {
    Dict_from_dict_ref(parent) = ac::Dict::array();
}

void ac_dict_set_object(ac_dict_ref parent) {
    Dict_from_dict_ref(parent) = ac::Dict::object();
}

void ac_dict_set_binary(ac_dict_ref parent, const uint8_t* data, uint32_t size) {
    Dict_from_dict_ref(parent) = ac::Dict::binary(ac::Blob(data, data + size));
}

ac_dict_ref ac_dict_add_child(ac_dict_ref parent, const char* key) {
    return dict_try_catch([&] {
        if (key) {
            return Dict_to_dict_ref(Dict_from_dict_ref(parent).operator[](key));
        }
        else {
            return Dict_to_dict_ref(Dict_from_dict_ref(parent).emplace_back(ac::Dict()));
        }
    });
}

char* ac_dict_dump(ac_dict_ref d, int indent) {
    auto dump = Dict_from_dict_ref(d).dump(indent);
    return strdup(dump.c_str());
}

int ac_dict_dump_to(ac_dict_ref d, int indent, char* buf, int buf_size) {
    auto dump = Dict_from_dict_ref(d).dump(indent);
    const int s = int(dump.size());
    if (s < buf_size) {
        memcpy(buf, dump.c_str(), dump.size() + 1);
    }
    return s;
}

} // extern "C"
