// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <ac/dict_root.h>
#include <ac/api_provider.h>

#if defined(__cplusplus)
extern "C" {
#endif

AC_LOCAL_EXPORT ac_api_provider* ac_new_local_api_provider();
AC_LOCAL_EXPORT void ac_add_model(
    ac_api_provider* local_provider,
    const char* model_id, const char* inferenceType, ac_dict_root* dict
);

#if defined(__cplusplus)
}
#endif
