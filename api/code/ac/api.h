// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "api_provider.h"
#include "dict_ref.h"
#include "dict_root.h"

// functions implemented in c-api.cpp

// general rules:
// the ownership of dict_root is transferred to the underlying C++ code
// the result_cb will be called with an error message if there is an error (and the payload will be null)
// progress_cb can be null

#if defined(__cplusplus)
extern "C" {
#endif

// to obtain a provider use a library which can create it
AC_API_EXPORT void ac_free_api_provider(ac_api_provider* p);

typedef struct ac_model ac_model;
AC_API_EXPORT void ac_free_model(ac_model* m);
AC_API_EXPORT void ac_create_model_json_params(
    ac_api_provider* p,
    const char* model_id,
    ac_dict_root* dict_root,
    void (*result_cb)(ac_model* m, const char* error, void* user_data),
    void (*progress_cb)(float progress, void* user_data),
    void* cb_user_data
);

typedef struct ac_instance ac_instance;
AC_API_EXPORT void ac_free_instance(ac_instance* i);
AC_API_EXPORT void ac_create_instance_json_params(
    ac_model* m,
    const char* instance_type,
    ac_dict_root* dict_root,
    void (*result_cb)(ac_instance* i, const char* error, void* user_data),
    void (*progress_cb)(float progress, void* user_data),
    void* cb_user_data
);

AC_API_EXPORT void ac_run_op_json_params(
    ac_instance* i,
    const char* op,
    ac_dict_root* dict_root,
    void (*result_cb)(const char* error, void* user_data),
    void (*stream_cb)(ac_dict_ref dict, void* user_data),
    void* cb_user_data
);

AC_API_EXPORT void ac_synchronize_instance(ac_instance* i);

AC_API_EXPORT void ac_initiate_instance_abort(
    ac_instance* i,
    void (*done_cb)(const char* error, void* user_data),
    void* cb_user_data
);

#if defined(__cplusplus)
}
#endif
