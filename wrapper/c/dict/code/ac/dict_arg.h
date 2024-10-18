// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "dict_ref.h"
#include <ac/inline.h>
#include <stdbool.h>

/// @ingroup c-dict
/// @{

/// A bundle of an @ref ac_dict_ref and its intent.
/// Used by various functions which take `dict`-s as arguments.
/// Not indented to be initialized directly, but rather through the helper functions:
/// - @ref ac_dict_arg_copy
/// - @ref ac_dict_arg_take
/// - @ref ac_dict_arg_null
typedef struct ac_dict_arg {
    ac_dict_ref ref;
    bool copy; // or take (move) if false
} ac_dict_arg;

/// Deep copies the ref.
/// The input ref is untouched and instead the target will perform a deep copy of the value inside.
AC_INLINE ac_dict_arg ac_dict_arg_copy(ac_dict_ref ref) {
    ac_dict_arg ret = {ref, true};
    return ret;
}

/// Moves the ref.
/// The input ref is moved into the target and is then set to null.
AC_INLINE ac_dict_arg ac_dict_arg_take(ac_dict_ref ref) {
    ac_dict_arg ret = {ref, false};
    return ret;
}

/// Creates a null (empty) arg.
/// This is useful for functions which take optional or empty dicts and saves the need to allocate and free a new
/// @ref ac_dict_root and make an @ref ac_dict_ref from it.
AC_INLINE ac_dict_arg ac_dict_arg_null(void) {
    ac_dict_arg ret = {0};
    return ret;
}

/// @}
