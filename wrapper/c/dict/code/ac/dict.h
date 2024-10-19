/**
 * @file dict.h
 * @brief Provides C API for dictionary operations in the Alpaca Core library.
 *
 * This file contains function declarations for creating, manipulating, and querying
 * dictionary structures. It provides a C interface to the underlying C++ implementation
 * based on the nlohmann::json library, allowing for flexible and efficient handling
 * of JSON-like data structures.
 */

// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "dict_ref.h"
#include "dict_root.h"
#include "dict_arg.h"
#include <ac/inline.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

/// @defgroup c-dict C Dictionary API
/// C API for dictionary operations in the Alpaca Core library.
/// The C dictionary is blah, blah, blah.

/// @addtogroup c-dict
/// @{

/// Get the last error message or `NULL` if no error.
/// The function returns a thread-local string. Ownership of the string is not transferred.
/// @note Every dict function invalidates the last error.
/// @warning Invalid dict ref arguments are not checked and will lead to crashes.
AC_C_DICT_EXPORT const char* ac_dict_get_last_error(void);

/// Create a new dict root.
/// The ownership of the result is transferred to the caller and it must be freed with @ref ac_dict_free_root.
///
/// The function returns `NULL` if the allocation fails.
AC_C_DICT_EXPORT ac_dict_root* ac_dict_new_root(void);

/// Free a dict root.
/// The function frees a root (and all its children). Calling it with `NULL` is safe.
AC_C_DICT_EXPORT void ac_dict_free_root(ac_dict_root* d);

/// Make a reference from a dict root.
AC_C_DICT_EXPORT ac_dict_ref ac_dict_make_ref(ac_dict_root* d);

/// Parse a JSON string and store the object in a dict ref.
/// The JSON string can be provided with a begin and an end.
/// If end is `NULL`, the string is assumed to be zero-terminated.
///
/// The function returns `NULL` on error.
///
/// The function does not merge. Any data previously in the dict ref is lost.
AC_C_DICT_EXPORT ac_dict_ref ac_dict_parse_json(ac_dict_ref target, const char* json, const char* json_end);

/// Deep-copy data from source dictionary to target.
AC_C_DICT_EXPORT void ac_dict_copy(ac_dict_ref target, ac_dict_ref source);

/// Move data from source dictionary to target.
/// The source dictionary is set to a valid empty state ("`null`" in JSON terms).
AC_C_DICT_EXPORT void ac_dict_take(ac_dict_ref target, ac_dict_ref source);

/// Transfer a dict from an arg to a ref respecting the arg intent.
AC_C_DICT_EXPORT void ac_dict_transfer(ac_dict_ref target, ac_dict_arg src);

/// Utility func to directly create a new root from a JSON string.
/// As with ac_dict_parse_json, the JSON string can be provided with a begin and an end.
/// If end is `NULL`, the string is assumed to be zero-terminated.
AC_INLINE ac_dict_root* ac_dict_new_root_from_json(const char* json, const char* json_end) {
    ac_dict_root* root = ac_dict_new_root();
    if (root) {
        if (!ac_dict_parse_json(ac_dict_make_ref(root), json, json_end)) {
            ac_dict_free_root(root);
            root = NULL;
        }
    }
    return root;
}

/// Utility func to directly create a new root from a dict arg, respecting the arg intent.
AC_INLINE ac_dict_root* ac_dict_new_root_from(ac_dict_arg source) {
    ac_dict_root* root = ac_dict_new_root();
    if (root) {
        ac_dict_transfer(ac_dict_make_ref(root), source);
    }
    return root;
}

/// Get a child by key.
/// The function returns `NULL` if the key is not found or the input is not an object.
AC_C_DICT_EXPORT ac_dict_ref ac_dict_at_key(ac_dict_ref d, const char* key);

/// Get a child by index.
/// The function returns `NULL` if the index is out of bounds or the input is not an array.
AC_C_DICT_EXPORT ac_dict_ref ac_dict_at_index(ac_dict_ref d, int index);

/// Dictionary value types.
typedef enum ac_dict_value_type {
    ac_dict_value_type_null,         /**< Null */
    ac_dict_value_type_bool,         /**< Boolean */
    ac_dict_value_type_number_int,   /**< 32-bit signed integer */
    ac_dict_value_type_number_unsigned, /**< 32-bit unsigned integer */
    ac_dict_value_type_number_double,/**< Double */
    ac_dict_value_type_string,       /**< String*/
    ac_dict_value_type_array,        /**< Array (posibly heterogeneous) values */
    ac_dict_value_type_object,       /**< Object (key-value pairs) */
    ac_dict_value_type_binary,       /**< Binary data */
} ac_dict_value_type;

/// Get the type of a dict ref.
AC_C_DICT_EXPORT ac_dict_value_type ac_dict_get_type(ac_dict_ref d);

/// Get the number of elements.
/// This function has no error conditions. It returns 0 for null and 1 for non-array/object types.
AC_C_DICT_EXPORT int ac_dict_get_size(ac_dict_ref d);

/// Get a boolean value.
/// Returns false if the input is not a boolean.
AC_C_DICT_EXPORT bool ac_dict_get_bool_value(ac_dict_ref d);

/// Get an integer value.
/// - Implicitly casts unsigned values smaller than `INT_MAX`.
/// - Implicitly casts whole floating point values.
/// - Returns 0 if a cast cannot be performed.
AC_C_DICT_EXPORT int ac_dict_get_int_value(ac_dict_ref d);

/// Get an unsigned integer value.
/// - Implicitly casts positive integer values.
/// - Implicitly casts whole floating point values.
/// - Returns 0 if a cast cannot be performed.
AC_C_DICT_EXPORT unsigned ac_dict_get_unsigned_value(ac_dict_ref d);

/// Get a double value.
/// - Implicitly casts integer values.
/// - Returns 0 if a cast cannot be performed.
AC_C_DICT_EXPORT double ac_dict_get_double_value(ac_dict_ref d);

/// Get a string value.
/// The ownership of the string is not transferred. Its lifetime is tied to the dict ref.
///
/// Resturns `NULL` if the input is not a string.
AC_C_DICT_EXPORT const char* ac_dict_get_string_value(ac_dict_ref d);

/// Binary buffer structure.
typedef struct ac_dict_binary_buf {
    uint8_t* data; /**< Pointer to binary data. */
    uint32_t size; /**< Size of binary data in bytes. */
} ac_dict_binary_buf;

/// Get a binary value.
/// The ownership of the binary data is not transferred. Its lifetime is tied to the dict ref.
///
/// Returns a buf with `NULL` data if the input is not binary.
AC_C_DICT_EXPORT ac_dict_binary_buf ac_dict_get_binary_value(ac_dict_ref d);

/// Opaque structure for dictionary iteration.
typedef struct ac_dict_iter ac_dict_iter;

/// Create a new iterator.
/// This function has no error conditions besides failing to allocate memory.
/// It returns `NULL` on a null value, but this is not an error, as `NULL` is a valid "end" iterator.
AC_C_DICT_EXPORT ac_dict_iter* ac_dict_new_iter(ac_dict_ref d);

/// Move an iterator to the next element.
/// The function returns `NULL` *and frees the iterator* if the iterator is at the end.
AC_C_DICT_EXPORT ac_dict_iter* ac_dict_iter_next(ac_dict_iter* it);

/// Free an iterator.
/// The function frees an incomplete iterator. Calling it with `NULL` is safe.
AC_C_DICT_EXPORT void ac_dict_free_iter(ac_dict_iter* it);

/// Get key from an iterator.
/// Returns `NULL` if not iterating over an object.
AC_C_DICT_EXPORT const char* ac_dict_iter_get_key(ac_dict_iter* d);

/// Get value from an iterator.
AC_C_DICT_EXPORT ac_dict_ref ac_dict_iter_get_value(ac_dict_iter* d);

/// Set dictionary value to null.
/// Any previous value is overwritten and lost.
AC_C_DICT_EXPORT void ac_dict_set_null(ac_dict_ref d);

/// Set dictionary value to a boolean value.
/// Any previous value is overwritten and lost.
AC_C_DICT_EXPORT void ac_dict_set_bool(ac_dict_ref d, bool value);

/// Set dictionary value to an integer.
/// Any previous value is overwritten and lost.
AC_C_DICT_EXPORT void ac_dict_set_int(ac_dict_ref d, int value);

/// Set dictionary value to an unsigned integer.
/// Any previous value is overwritten and lost.
AC_C_DICT_EXPORT void ac_dict_set_unsigned(ac_dict_ref d, unsigned value);

/// Set dictionary value to a double.
/// Any previous value is overwritten and lost.
AC_C_DICT_EXPORT void ac_dict_set_double(ac_dict_ref d, double value);

/// Set dictionary value to a string.
/// Any previous value is overwritten and lost.
///
/// The `end` parameter is optional and can be used to specify the end of the string.
/// If `end` is `NULL`, the string is assumed to be zero-terminated.
AC_C_DICT_EXPORT void ac_dict_set_string(ac_dict_ref d, const char* value, const char* end);

/// Set dictionary value to an empty array.
/// Any previous value is overwritten and lost.
AC_C_DICT_EXPORT void ac_dict_set_array(ac_dict_ref d);

/// Set dictionary value to an empty object.
/// Any previous value is overwritten and lost.
AC_C_DICT_EXPORT void ac_dict_set_object(ac_dict_ref d);

/// Set dictionary value to binary data.
/// Any previous value is overwritten and lost.
AC_C_DICT_EXPORT void ac_dict_set_binary(ac_dict_ref d, const uint8_t* data, uint32_t size);

/// Add a child to an array.
/// Returns the newly created child or `NULL` if the input is not compound.
/// For arrays, the child is appended to the end and `key` is ignored.
AC_C_DICT_EXPORT ac_dict_ref ac_dict_add_child(ac_dict_ref parent, const char* key);

/// Dump dict to string.
/// The `indent` parameter specifies the indentation level: -1 for compact, &ge; 0 for pretty.
///
/// The returned value is a newly allocated string that must be `free`-d by the caller.
/// The function returns `NULL` on error.
AC_C_DICT_EXPORT char* ac_dict_dump(ac_dict_ref d, int indent);

/// Dump dict to buffer.
/// The `indent` parameter specifies the indentation level: -1 for compact, 0 for pretty, 1+ for pretty with indent.
///
/// The function follows `snprintf` semantics and returns the number of characters that would have been written
/// (ignoring `buf_size`) or -1 on error. It only writes to the buffer if the return value is less than `buf_size`.
AC_C_DICT_EXPORT int ac_dict_dump_to(ac_dict_ref d, int indent, char* buf, int buf_size);

/// @}

#if defined(__cplusplus)
}
#endif
