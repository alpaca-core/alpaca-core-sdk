// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "dict.h"
#include "Dict.hpp"

#include <string.h>

struct ac_dict_root {
    ac::Dict dict;
};

namespace {

ac::Dict* r(ac_dict_ref ref) {
    return reinterpret_cast<ac::Dict*>(ref);
}

ac_dict_ref mr(ac::Dict& dict) {
    return reinterpret_cast<ac_dict_ref>(&dict);
}

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
}

const char* ac_dict_get_last_error() {
    if (dict_last_error.empty()) return nullptr;
    return dict_last_error.c_str();
}

extern "C" {
ac_dict_root* ac_dict_new_root() {
    return new ac_dict_root();
}

void ac_dict_free_root(ac_dict_root* d) {
    delete d;
}

ac_dict_root* ac_dict_new_root_from_json(const char* json, const char* json_end) {
    if (!json_end) {
        json_end = json + strlen(json);
    }
    return dict_try_catch([&] {
        return new ac_dict_root{ac::Dict::parse(json, json_end)};
    });
}

ac_dict_root* ac_dict_new_root_from_ref_copy(ac_dict_ref d) {
    return new ac_dict_root{*r(d)};
}

ac_dict_root* ac_dict_new_root_from_ref_take(ac_dict_ref d) {
    return new ac_dict_root{std::move(*r(d))};
}

ac_dict_ref ac_dict_make_ref(ac_dict_root* d) {
    return &d->dict;
}

ac_dict_ref ac_dict_at_key(ac_dict_ref d, const char* key) {
    return dict_try_catch([&] {
        return mr(r(d)->at(key));
    });
}

ac_dict_ref ac_dict_at_index(ac_dict_ref d, int index) {
    return dict_try_catch([&] {
        return mr(r(d)->at(index));
    });
}

ac_dict_value_type ac_dict_get_type(ac_dict_ref d) {
    switch (r(d)->type()) {
        case ac::Dict::value_t::null: return ac_value_type_null;
        case ac::Dict::value_t::boolean: return ac_value_type_bool;
        case ac::Dict::value_t::number_integer: return ac_value_type_number_int;
        case ac::Dict::value_t::number_unsigned: return ac_value_type_number_unsigned;
        case ac::Dict::value_t::number_float: return ac_value_type_number_double;
        case ac::Dict::value_t::string: return ac_value_type_string;
        case ac::Dict::value_t::array: return ac_value_type_array;
        case ac::Dict::value_t::object: return ac_value_type_object;
        default:
            dict_last_error = "Unsupported type";
            return ac_value_type_null;
    }
}

bool ac_dict_get_bool_value(ac_dict_ref d) {
    return dict_try_catch([&] {
        return r(d)->get<bool>();
    });
}

int ac_dict_get_int_value(ac_dict_ref d) {
    return dict_try_catch([&] {
        return r(d)->get<int>();
    });
}

unsigned ac_dict_get_unsigned_value(ac_dict_ref d) {
    return dict_try_catch([&] {
        return r(d)->get<unsigned>();
    });
}

double ac_dict_get_double_value(ac_dict_ref d) {
    return dict_try_catch([&] {
        return r(d)->get<double>();
    });
}

const char* ac_dict_get_string_value(ac_dict_ref d) {
    return dict_try_catch([&] {
        return r(d)->get<std::string_view>().data();
    });
}

ac_dict_binary_buf ac_dict_get_binary_value(ac_dict_ref d) {
    return dict_try_catch([&] {
        auto& bin = r(d)->get_binary();
        return ac_dict_binary_buf{bin.data(), uint32_t(bin.size())};
    });
}

} // extern "C"