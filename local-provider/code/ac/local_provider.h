// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <ac/api_provider.h>

#if defined(__cplusplus)
extern "C" {
#endif

AC_LOCAL_EXPORT ac_api_provider* ac_new_local_api_provider();

typedef struct ac_model_info_asset {
    const char* id;
    const char* tag;
} ac_model_info_asset;
AC_LOCAL_EXPORT void ac_add_model(
    ac_api_provider* local_provider,
    const char* model_id,
    const char* inferenceType,
    ac_model_info_asset* assets,
    size_t assets_count
);

#if defined(__cplusplus)
}
#endif
