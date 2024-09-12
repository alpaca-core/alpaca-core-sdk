// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once


#ifdef GENERATE_SCHEMA
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <string>

template<typename T>
inline void write_param(nlohmann::json& obj, const std::string& name, const std::string& type, const std::string& description, const T& default_value) {
    obj[name] = {
        {"type", type},
        {"description", description},
        {"default", default_value}
    };
}


inline std::string map_type(const std::string& cpp_type) {
    if (cpp_type == "std::string") return "string";
    if (cpp_type == "int") return "integer";
    if (cpp_type == "float") return "number";
    if (cpp_type == "bool") return "boolean";
    return cpp_type; // fallback
}
#else
#include "ac/Dict.hpp"
#endif

#ifdef GENERATE_SCHEMA

#define DEFINE_PARAM(name, type, description, default_value) \
    write_param(__params, #name, map_type(#type), description, default_value);

#define DEFINE_RESULT(name, type, description) \
    write_param(__result, #name, map_type(#type), description, "");

#else
#define DEFINE_PARAM(name, type, description, default_value) \
  static type name(const ::ac::Dict& d) { return ::ac::Dict_optValueAt<type>(d, #name, default_value); } \
  static void set_##name(::ac::Dict& d, type value) { d[#name] = value; }

#define DEFINE_RESULT(name, type, description) \
    static type name(const ::ac::Dict& d) { return d[#name].get<type>(); } \
    static void set_##name(::ac::Dict& d, type value) { d[#name] = value; }
#endif

#define BEGIN_SCHEMA(name) \
    GENERATE_SCHEMA_BEGIN(name)                     \
    STRUCT_BEGIN(name)

#define END_SCHEMA() \
    STRUCT_END() \
    GENERATE_SCHEMA_END()

#define BEGIN_PARAMS() \
    GENERATE_SCHEMA_PARAMS_BEGIN() \
    STRUCT_PARAMS_BEGIN()

#define END_PARAMS() \
    GENERATE_SCHEMA_PARAMS_END() \
    STRUCT_PARAMS_END()

#define BEGIN_RESULT() \
    GENERATE_SCHEMA_RESULT_BEGIN() \
    STRUCT_RESULT_BEGIN()

#define END_RESULT() \
    GENERATE_SCHEMA_RESULT_END() \
    STRUCT_RESULT_END()

#ifdef GENERATE_SCHEMA
#define GENERATE_SCHEMA_BEGIN(name) nlohmann::json name() { nlohmann::json schema;
#define GENERATE_SCHEMA_END() return schema; }
#define GENERATE_SCHEMA_PARAMS_BEGIN() nlohmann::json __params = nlohmann::json::object();
#define GENERATE_SCHEMA_PARAMS_END() schema["params"] = __params;
#define GENERATE_SCHEMA_RESULT_BEGIN() nlohmann::json __result = nlohmann::json::object();
#define GENERATE_SCHEMA_RESULT_END() schema["result"] = __result;
#define STRUCT_BEGIN(name)
#define STRUCT_END()
#define STRUCT_PARAMS_BEGIN()
#define STRUCT_PARAMS_END()
#define STRUCT_RESULT_BEGIN()
#define STRUCT_RESULT_END()
#else
#define GENERATE_SCHEMA_BEGIN(name)
#define GENERATE_SCHEMA_END()
#define GENERATE_SCHEMA_PARAMS_BEGIN()
#define GENERATE_SCHEMA_PARAMS_END()
#define GENERATE_SCHEMA_RESULT_BEGIN()
#define GENERATE_SCHEMA_RESULT_END()
#define STRUCT_BEGIN(name) struct name {
#define STRUCT_END() };
#define STRUCT_PARAMS_BEGIN() struct RunParams {
#define STRUCT_PARAMS_END() };
#define STRUCT_RESULT_BEGIN() struct RunResult {
#define STRUCT_RESULT_END() };
#endif
