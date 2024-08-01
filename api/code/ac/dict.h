// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <stdbool.h>
#include <stdint.h>

// functions implemented in c-api.cpp

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct ac_dict_root ac_dict_root;
typedef void* ac_dict_ref;

// return null when there is no error
// otherwise return a thread-local string with the error
// every dict function invalidates the previous error
// note, every dict function which takes a dict_ref expects a valid one and will just UB (likely crash) on invalid refs
AC_API_EXPORT const char* ac_dict_get_last_error();

AC_API_EXPORT ac_dict_root* ac_dict_new_root();
AC_API_EXPORT void ac_dict_free_root(ac_dict_root* d);

// set json_end to nullptr to treat the string as zero-terminated
AC_API_EXPORT ac_dict_root* ac_dict_new_root_from_json(const char* json, const char* json_end);

// deep copy of the dict pointed by ref into a new root
AC_API_EXPORT ac_dict_root* ac_dict_new_root_from_ref_copy(ac_dict_ref d);

// creat a new root from the ref taking the data, leaving ref null
AC_API_EXPORT ac_dict_root* ac_dict_new_root_from_ref_take(ac_dict_ref d);

// return a ref to the dict
AC_API_EXPORT ac_dict_ref ac_dict_make_ref(ac_dict_root* d);

AC_API_EXPORT ac_dict_ref ac_dict_at_key(ac_dict_ref d, const char* key); // return null if not found
AC_API_EXPORT ac_dict_ref ac_dict_at_index(ac_dict_ref d, int index); // return null if not found

typedef enum ac_dict_value_type {
    ac_dict_value_type_null,
    ac_dict_value_type_bool,
    ac_dict_value_type_number_int,
    ac_dict_value_type_number_unsigned,
    ac_dict_value_type_number_double,
    ac_dict_value_type_string,
    ac_dict_value_type_array,
    ac_dict_value_type_object,
    ac_dict_value_type_binary,
} ac_dict_value_type;

AC_API_EXPORT ac_dict_value_type ac_dict_get_type(ac_dict_ref d);

// return
// 0 for null
// 1 for bool, number, string
// number of elements for array and object
AC_API_EXPORT int ac_dict_get_size(ac_dict_ref d);

AC_API_EXPORT bool ac_dict_get_bool_value(ac_dict_ref d); // silently fails returning false
AC_API_EXPORT int ac_dict_get_int_value(ac_dict_ref d); // silently fails returning 0
AC_API_EXPORT unsigned ac_dict_get_unsigned_value(ac_dict_ref d); // silently fails returning 0
AC_API_EXPORT double ac_dict_get_double_value(ac_dict_ref d); // silently fails returning 0
AC_API_EXPORT const char* ac_dict_get_string_value(ac_dict_ref d); // return null if not a string

typedef struct ac_dict_binary_buf {
    uint8_t* data;
    uint32_t size;
} ac_dict_binary_buf;

// the returned buffer is owned by the dict
// data is null on error
AC_API_EXPORT ac_dict_binary_buf ac_dict_get_binary_value(ac_dict_ref d);

// iterate
typedef struct ac_dict_iter ac_dict_iter;
AC_API_EXPORT ac_dict_iter* ac_dict_new_iter(ac_dict_ref d); // create iterator pointing to the first element

// when there are no more elements free and invalidate iterator and return null
// otherwise return the iterator
AC_API_EXPORT ac_dict_iter* ac_dict_iter_next(ac_dict_iter* it);
AC_API_EXPORT void ac_dict_free_iter(ac_dict_iter* it); // free iterator, safe to call on null
AC_API_EXPORT const char* ac_dict_iter_get_key(ac_dict_iter* d); // return null if not an object
AC_API_EXPORT ac_dict_ref ac_dict_iter_get_value(ac_dict_iter* d);

// create

AC_API_EXPORT void ac_dict_set_null(ac_dict_ref parent);
AC_API_EXPORT void ac_dict_set_bool(ac_dict_ref parent, bool value);
AC_API_EXPORT void ac_dict_set_int(ac_dict_ref parent, int value);
AC_API_EXPORT void ac_dict_set_unsigned(ac_dict_ref parent, unsigned value);
AC_API_EXPORT void ac_dict_set_double(ac_dict_ref parent, double value);
AC_API_EXPORT void ac_dict_set_string(ac_dict_ref parent, const char* value);
AC_API_EXPORT void ac_dict_set_array(ac_dict_ref parent); // set to empty array
AC_API_EXPORT void ac_dict_set_object(ac_dict_ref parent); // set to empty object
AC_API_EXPORT void ac_dict_set_binary(ac_dict_ref parent, const uint8_t* data, uint32_t size); // copy data to dict

// set key as null to make array elements
// return a null value on error
AC_API_EXPORT ac_dict_ref ac_dict_add_child(ac_dict_ref parent, const char* key);

#if defined(__cplusplus)
}
#endif
