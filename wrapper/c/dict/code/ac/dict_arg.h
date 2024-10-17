// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "dict_ref.h"
#include <ac/inline.h>
#include <stdbool.h>

/// \ingroup c-dict
// @{

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

// @}
