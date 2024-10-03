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
#include <ac/inline.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Get the last error message.
 *
 * @return const char* Null if no error, otherwise a thread-local error string.
 * @note Every dict function invalidates the previous error.
 * @warning Every dict function expecting a dict_ref will likely crash on invalid refs.
 */
AC_C_DICT_EXPORT const char* ac_dict_get_last_error(void);

/**
 * @brief Create a new dictionary root.
 *
 * This function creates a new ac_dict_root structure, which internally contains
 * an instance of the C++ ac::Dict type (an alias for nlohmann::json).
 *
 * @return ac_dict_root* Pointer to the new dictionary root, or NULL on error.
 * @note This function uses exception handling internally. Check ac_dict_get_last_error() for error details.
 */
AC_C_DICT_EXPORT ac_dict_root* ac_dict_new_root(void);

/**
 * @brief Free a dictionary root.
 *
 * @param d Pointer to the dictionary root to free.
 */
AC_C_DICT_EXPORT void ac_dict_free_root(ac_dict_root* d);

/**
 * @brief Create a reference to a dictionary.
 *
 * This function creates an ac_dict_ref, which is an opaque pointer type
 * representing a reference to a dictionary object. Internally, it corresponds
 * to a pointer to the C++ ac::Dict type.
 *
 * @param d Pointer to the dictionary root.
 * @return ac_dict_ref Reference to the dictionary.
 */
AC_C_DICT_EXPORT ac_dict_ref ac_dict_make_ref(ac_dict_root* d);

/**
 * @brief Parse JSON into a dictionary.
 *
 * This function parses a JSON string and stores the result in the target dictionary.
 * It uses the underlying nlohmann::json parser for efficient and flexible parsing.
 *
 * @param target Target dictionary reference.
 * @param json JSON string to parse.
 * @param json_end End of JSON string (NULL for zero-terminated).
 * @return bool True if parsing succeeded, false otherwise.
 * @note This function uses exception handling internally. Check ac_dict_get_last_error() for error details.
 */
AC_C_DICT_EXPORT bool ac_dict_parse_json(ac_dict_ref target, const char* json, const char* json_end);

/**
 * @brief Deep copy a dictionary.
 *
 * @param target Target dictionary reference.
 * @param source Source dictionary reference.
 */
AC_C_DICT_EXPORT void ac_dict_copy(ac_dict_ref target, ac_dict_ref source);

/**
 * @brief Move data from source dictionary to target.
 *
 * @param target Target dictionary reference.
 * @param source Source dictionary reference (becomes null after move).
 */
AC_C_DICT_EXPORT void ac_dict_take(ac_dict_ref target, ac_dict_ref source);

typedef struct ac_dict_arg {
    ac_dict_ref ref;
    bool copy; // or take (move) if false
} ac_dict_arg;

AC_INLINE ac_dict_arg ac_dict_arg_copy(ac_dict_ref ref) {
    ac_dict_arg ret = {ref, true};
    return ret;
}

AC_INLINE ac_dict_arg ac_dict_arg_take(ac_dict_ref ref) {
    ac_dict_arg ret = {ref, false};
    return ret;
}

AC_INLINE ac_dict_arg ac_dict_arg_null() {
    ac_dict_arg ret = {0};
    return ret;
}

AC_C_DICT_EXPORT void ac_dict_transfer(ac_dict_ref target, ac_dict_arg src);

/**
 * @brief Create a new dictionary root from JSON.
 *
 * @param json JSON string to parse.
 * @param json_end End of JSON string (null for zero-terminated).
 * @return ac_dict_root* New dictionary root, or null on error.
 * @note This function uses exception handling internally. Check ac_dict_get_last_error() for error details.
 */
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

AC_INLINE ac_dict_root* ac_dict_new_root_from(ac_dict_arg source) {
    ac_dict_root* root = ac_dict_new_root();
    if (root) {
        ac_dict_transfer(ac_dict_make_ref(root), source);
    }
    return root;
}

/**
 * @brief Get element at key of object.
 *
 * @param d Dictionary reference.
 * @param key Key to look up.
 * @return ac_dict_ref Reference to element, or null on error.
 * @note This function uses exception handling internally. Check ac_dict_get_last_error() for error details.
 */
AC_C_DICT_EXPORT ac_dict_ref ac_dict_at_key(ac_dict_ref d, const char* key);

/**
 * @brief Get element at index of array.
 *
 * @param d Dictionary reference.
 * @param index Index to look up.
 * @return ac_dict_ref Reference to element, or null on error.
 * @note This function uses exception handling internally. Check ac_dict_get_last_error() for error details.
 */
AC_C_DICT_EXPORT ac_dict_ref ac_dict_at_index(ac_dict_ref d, int index);

/**
 * @brief Enumeration of possible dictionary value types.
 *
 * This enumeration corresponds to the various types that can be stored in
 * the underlying nlohmann::json object, including special support for binary data.
 */
typedef enum ac_dict_value_type {
    ac_dict_value_type_null,         /**< Null value */
    ac_dict_value_type_bool,         /**< Boolean value */
    ac_dict_value_type_number_int,   /**< Integer number */
    ac_dict_value_type_number_unsigned, /**< Unsigned integer number */
    ac_dict_value_type_number_double,/**< Floating-point number */
    ac_dict_value_type_string,       /**< String value */
    ac_dict_value_type_array,        /**< Array of values */
    ac_dict_value_type_object,       /**< Object (key-value pairs) */
    ac_dict_value_type_binary,       /**< Binary data */
} ac_dict_value_type;

/**
 * @brief Get the type of a dictionary value.
 *
 * @param d Dictionary reference.
 * @return ac_dict_value_type Type of the dictionary value.
 * @note This function uses exception handling internally. Check ac_dict_get_last_error() for error details.
 */
AC_C_DICT_EXPORT ac_dict_value_type ac_dict_get_type(ac_dict_ref d);

/**
 * @brief Get the size of a dictionary value.
 *
 * @param d Dictionary reference.
 * @return int 0 for null, 1 for bool/number/string, number of elements for array/object.
 */
AC_C_DICT_EXPORT int ac_dict_get_size(ac_dict_ref d);

/**
 * @brief Get boolean value from dictionary.
 *
 * @param d Dictionary reference.
 * @return bool Boolean value (false on error).
 * @note This function uses exception handling internally. Check ac_dict_get_last_error() for error details.
 */
AC_C_DICT_EXPORT bool ac_dict_get_bool_value(ac_dict_ref d);

/**
 * @brief Get integer value from dictionary.
 *
 * @param d Dictionary reference.
 * @return int Integer value (0 on error).
 * @note This function uses exception handling internally. Check ac_dict_get_last_error() for error details.
 */
AC_C_DICT_EXPORT int ac_dict_get_int_value(ac_dict_ref d);

/**
 * @brief Get unsigned integer value from dictionary.
 *
 * @param d Dictionary reference.
 * @return unsigned Unsigned integer value (0 on error).
 * @note This function uses exception handling internally. Check ac_dict_get_last_error() for error details.
 */
AC_C_DICT_EXPORT unsigned ac_dict_get_unsigned_value(ac_dict_ref d);

/**
 * @brief Get double value from dictionary.
 *
 * @param d Dictionary reference.
 * @return double Double value (0.0 on error).
 * @note This function uses exception handling internally. Check ac_dict_get_last_error() for error details.
 */
AC_C_DICT_EXPORT double ac_dict_get_double_value(ac_dict_ref d);

/**
 * @brief Get string value from dictionary.
 *
 * @param d Dictionary reference.
 * @return const char* String value (null if not a string).
 * @note This function uses exception handling internally. Check ac_dict_get_last_error() for error details.
 */
AC_C_DICT_EXPORT const char* ac_dict_get_string_value(ac_dict_ref d);

/**
 * @brief Structure representing a binary buffer.
 *
 * This structure is used to handle binary data in the dictionary,
 * corresponding to the ac::Blob type in the C++ implementation.
 */
typedef struct ac_dict_binary_buf {
    uint8_t* data; /**< Pointer to binary data. */
    uint32_t size; /**< Size of binary data in bytes. */
} ac_dict_binary_buf;

/**
 * @brief Get binary value from dictionary.
 *
 * @param d Dictionary reference.
 * @return ac_dict_binary_buf Binary buffer (data is null on error).
 * @note The returned buffer is owned by the dictionary.
 * @note This function uses exception handling internally. Check ac_dict_get_last_error() for error details.
 */
AC_C_DICT_EXPORT ac_dict_binary_buf ac_dict_get_binary_value(ac_dict_ref d);

/**
 * @brief Opaque structure for dictionary iteration.
 */
typedef struct ac_dict_iter ac_dict_iter;

/**
 * @brief Create a new iterator for a dictionary.
 *
 * @param d Dictionary reference.
 * @return ac_dict_iter* New iterator pointing to the first element.
 */
AC_C_DICT_EXPORT ac_dict_iter* ac_dict_new_iter(ac_dict_ref d);

/**
 * @brief Move iterator to next element.
 *
 * @param it Iterator.
 * @return ac_dict_iter* Iterator (null if no more elements).
 */
AC_C_DICT_EXPORT ac_dict_iter* ac_dict_iter_next(ac_dict_iter* it);

/**
 * @brief Free an iterator.
 *
 * @param it Iterator to free (safe to call with null).
 */
AC_C_DICT_EXPORT void ac_dict_free_iter(ac_dict_iter* it);

/**
 * @brief Get key from iterator.
 *
 * @param d Iterator.
 * @return const char* Key (null if not an object).
 * @note This function uses exception handling internally. Check ac_dict_get_last_error() for error details.
 */
AC_C_DICT_EXPORT const char* ac_dict_iter_get_key(ac_dict_iter* d);

/**
 * @brief Get value from iterator.
 *
 * @param d Iterator.
 * @return ac_dict_ref Reference to value.
 */
AC_C_DICT_EXPORT ac_dict_ref ac_dict_iter_get_value(ac_dict_iter* d);

/**
 * @brief Set dictionary value to null.
 *
 * @param parent Dictionary reference.
 */
AC_C_DICT_EXPORT void ac_dict_set_null(ac_dict_ref parent);

/**
 * @brief Set dictionary value to boolean.
 *
 * @param parent Dictionary reference.
 * @param value Boolean value.
 */
AC_C_DICT_EXPORT void ac_dict_set_bool(ac_dict_ref parent, bool value);

/**
 * @brief Set dictionary value to integer.
 *
 * @param parent Dictionary reference.
 * @param value Integer value.
 */
AC_C_DICT_EXPORT void ac_dict_set_int(ac_dict_ref parent, int value);

/**
 * @brief Set dictionary value to unsigned integer.
 *
 * @param parent Dictionary reference.
 * @param value Unsigned integer value.
 */
AC_C_DICT_EXPORT void ac_dict_set_unsigned(ac_dict_ref parent, unsigned value);

/**
 * @brief Set dictionary value to double.
 *
 * @param parent Dictionary reference.
 * @param value Double value.
 */
AC_C_DICT_EXPORT void ac_dict_set_double(ac_dict_ref parent, double value);

/**
 * @brief Set dictionary value to string.
 *
 * @param parent Dictionary reference.
 * @param value String value.
 * @param end End of string (null for zero-terminated).
 */
AC_C_DICT_EXPORT void ac_dict_set_string(ac_dict_ref parent, const char* value, const char* end);

/**
 * @brief Set dictionary value to empty array.
 *
 * @param parent Dictionary reference.
 */
AC_C_DICT_EXPORT void ac_dict_set_array(ac_dict_ref parent);

/**
 * @brief Set dictionary value to empty object.
 *
 * @param parent Dictionary reference.
 */
AC_C_DICT_EXPORT void ac_dict_set_object(ac_dict_ref parent);

/**
 * @brief Set dictionary value to binary data.
 *
 * @param parent Dictionary reference.
 * @param data Binary data.
 * @param size Size of binary data.
 */
AC_C_DICT_EXPORT void ac_dict_set_binary(ac_dict_ref parent, const uint8_t* data, uint32_t size);

/**
 * @brief Add a child to a dictionary.
 *
 * @param parent Dictionary reference.
 * @param key Key for the child (null for array elements).
 * @return ac_dict_ref Reference to the new child (null on error).
 * @note This function uses exception handling internally. Check ac_dict_get_last_error() for error details.
 */
AC_C_DICT_EXPORT ac_dict_ref ac_dict_add_child(ac_dict_ref parent, const char* key);

/**
 * @brief Dump dictionary to string.
 *
 * @param d Dictionary reference.
 * @param indent Indentation (-1 for compact, 0 for pretty, 1+ for pretty with indent).
 * @return char* Dumped string (null on error, caller must free).
 * @note This function uses exception handling internally. Check ac_dict_get_last_error() for error details.
 */
AC_C_DICT_EXPORT char* ac_dict_dump(ac_dict_ref d, int indent);

/**
 * @brief Dump dictionary to buffer.
 *
 * @param d Dictionary reference.
 * @param indent Indentation (-1 for compact, 0 for pretty, 1+ for pretty with indent).
 * @param buf Buffer to write to.
 * @param buf_size Size of buffer.
 * @return int Number of characters that would have been written (ignoring buf_size), or -1 on error.
 * @note This function uses exception handling internally. Check ac_dict_get_last_error() for error details.
 */
AC_C_DICT_EXPORT int ac_dict_dump_to(ac_dict_ref d, int indent, char* buf, int buf_size);

#if defined(__cplusplus)
}
#endif
