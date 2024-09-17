// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <ac/asset/source.h>
#include <ac/dict_root.h>
#include <ac/model.h>
#include <astl/sv.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct ac_local_provider ac_local_provider;

AC_LOCAL_EXPORT ac_local_provider* ac_new_local_api_provider();

AC_LOCAL_EXPORT void ac_free_local_provider(ac_local_provider* p);

AC_LOCAL_EXPORT void ac_add_asset_source(ac_local_provider* local_provider, ac_asset_source* asset_source, int priority);

typedef struct ac_model_info_asset {
    const char* id;
    const char* tag;
} ac_model_info_asset;
AC_LOCAL_EXPORT void ac_add_model(
    ac_local_provider* local_provider,
    const char* model_id,
    const char* inferenceType,
    ac_model_info_asset* assets,
    size_t assets_count
);

/**
 * @brief Create a model asynchronously
 *
 * This function initiates the asynchronous creation of a model. The result is returned
 * via the provided callback function.
 *
 * @param p Pointer to the API provider
 * @param model_id Identifier for the model to be created (e.g., "gpt2")
 * @param dict_root Dictionary containing model parameters (can be NULL for default parameters)
 * @param result_cb Callback function to be called with the result
 * @param progress_cb Callback function for progress updates (can be NULL if not needed)
 * @param cb_user_data User data to be passed to the callbacks
 */
AC_LOCAL_EXPORT void ac_create_model(
    ac_local_provider* p,
    const char* model_id,
    ac_dict_root* dict_root,
    void (*result_cb)(ac_model* m, const char* error, void* user_data),
    void (*progress_cb)(ac_sv tag, float progress, void* user_data),
    void* cb_user_data
);

#if defined(__cplusplus)
}
#endif
