// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "dict.h"
#include "api.h"
#include "Dict.hpp"
#include "Instance.hpp"
#include "Model.hpp"
#include "Provider.hpp"

#include <astl/move.hpp>
#include <splat/warnings.h>

#include <string.h>

DISABLE_MSVC_WARNING(4996) // posix name deprecated

struct ac_dict_root {
    ac::Dict dict;
};

struct ac_dict_iter {
    ac::Dict::iterator it;
    ac::Dict::iterator end;
};

struct ac_model {
    ac::ModelPtr model;
};

struct ac_instance {
    ac::InstancePtr instance;
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

ac::Dict ac_dict_parse(const char* json, const char* json_end) {
    if (!json_end) {
        json_end = json + strlen(json);
    }
    return ac::Dict::parse(json, json_end);
}

} // namespace

///////////////////////////////////////////////
// dict

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

bool ac_dict_parse_json(ac_dict_ref target, const char* json, const char* json_end) {
    return dict_try_catch([&] {
        *r(target) = ac_dict_parse(json, json_end);
        return true;
    });
}

void ac_dict_copy(ac_dict_ref target, ac_dict_ref source) {
    *r(target) = *r(source);
}

void ac_dict_take(ac_dict_ref target, ac_dict_ref source) {
    *r(target) = astl::move(*r(source));
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
    return int(r(d)->size());
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

ac_dict_iter* ac_dict_new_iter(ac_dict_ref d) {
    return new ac_dict_iter{r(d)->begin(), r(d)->end()};
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
    return mr(it->it.value());
}

void ac_dict_set_null(ac_dict_ref parent) {
    *r(parent) = nullptr;
}

void ac_dict_set_bool(ac_dict_ref parent, bool value) {
    *r(parent) = value;
}

void ac_dict_set_int(ac_dict_ref parent, int value) {
    *r(parent) = value;
}

void ac_dict_set_unsigned(ac_dict_ref parent, unsigned value) {
    *r(parent) = value;
}

void ac_dict_set_double(ac_dict_ref parent, double value) {
    *r(parent) = value;
}

void ac_dict_set_string(ac_dict_ref parent, const char* value, const char* end) {
    if (!end) {
        end = value + strlen(value);
    }
    *r(parent) = std::string(value, end);
}

void ac_dict_set_array(ac_dict_ref parent) {
    *r(parent) = ac::Dict::array();
}

void ac_dict_set_object(ac_dict_ref parent) {
    *r(parent) = ac::Dict::object();
}

void ac_dict_set_binary(ac_dict_ref parent, const uint8_t* data, uint32_t size) {
    *r(parent) = ac::Dict::binary(ac::Blob(data, data + size));
}

ac_dict_ref ac_dict_add_child(ac_dict_ref parent, const char* key) {
    return dict_try_catch([&] {
        if (key) {
            return mr(r(parent)->operator[](key));
        }
        else {
            return mr(r(parent)->emplace_back(ac::Dict()));
        }
    });
}

char* ac_dict_dump(ac_dict_ref d, int indent) {
    auto dump = r(d)->dump(indent);
    return strdup(dump.c_str());
}

int ac_dict_dump_to(ac_dict_ref d, int indent, char* buf, int buf_size) {
    auto dump = r(d)->dump(indent);
    const int s = int(dump.size());
    if (s < buf_size) {
        memcpy(buf, dump.c_str(), dump.size() + 1);
    }
    return s;
}

///////////////////////////////////////////////
// api

void ac_free_api_provider(ac_api_provider* p) {
    auto provider = reinterpret_cast<ac::Provider*>(p);
    delete provider;
}

void ac_free_model(ac_model* m) {
    delete m;
}

void ac_create_model_json_params(
    ac_api_provider* p,
    const char* json,
    const char* json_end,
    void (*result_cb)(ac_model* m, const char* error, void* user_data),
    void (*progress_cb)(float progress, void* user_data),
    void* cb_user_data
) {
    auto provider = reinterpret_cast<ac::Provider*>(p);
    provider->createModel(ac_dict_parse(json, json_end), {
        [=](ac::CallbackResult<ac::ModelPtr> result) {
            if (result.has_value()) {
                result_cb(new ac_model{astl::move(result.value())}, nullptr, cb_user_data);
            }
            else {
                result_cb(nullptr, result.error().text.c_str(), cb_user_data);
            }
        },
        [=](float progress) {
            if (progress_cb) {
                progress_cb(progress, cb_user_data);
            }
        }
    });
}

void ac_free_instance(ac_instance* i) {
    delete i;
}

void ac_create_instance_json_params(
    ac_model* m,
    const char* instance_type,
    const char* json,
    const char* json_end,
    void (*result_cb)(ac_instance* i, const char* error, void* user_data),
    void (*progress_cb)(float progress, void* user_data),
    void* cb_user_data
) {
    m->model->createInstance(instance_type, ac_dict_parse(json, json_end), {
        [=](ac::CallbackResult<ac::InstancePtr> result) {
            if (result.has_value()) {
                result_cb(new ac_instance{astl::move(result.value())}, nullptr, cb_user_data);
            }
            else {
                result_cb(nullptr, result.error().text.c_str(), cb_user_data);
            }
        },
        [=](float progress) {
            if (progress_cb) {
                progress_cb(progress, cb_user_data);
            }
        }
    });
}

void ac_run_op_json_params(
    ac_instance* i,
    const char* op,
    const char* json,
    const char* json_end,
    void (*result_cb)(const char* error, void* user_data),
    void (*stream_cb)(ac_dict_ref dict, void* user_data),
    void* cb_user_data
) {
    i->instance->runOp(op, ac_dict_parse(json, json_end), {
        [=](ac::CallbackResult<void> result) {
            if (result.has_value()) {
                result_cb(nullptr, cb_user_data);
            }
            else {
                result_cb(result.error().text.c_str(), cb_user_data);
            }
        },
        [=](ac::Dict dict) {
            if (stream_cb) {
                stream_cb(mr(dict), cb_user_data);
            }
        }
    });
}

void ac_synchronize_instance(ac_instance* i) {
    i->instance->synchronize();
}

void ac_initiate_instance_abort(
    ac_instance* i,
    void (*done_cb)(const char* error, void* user_data),
    void* cb_user_data
) {
    i->instance->initiateAbort({
        [=](ac::CallbackResult<void> result) {
            if (result.has_value()) {
                done_cb(nullptr, cb_user_data);
            }
            else {
                done_cb(result.error().text.c_str(), cb_user_data);
            }
        },
        {}
    });
}

} // extern "C"