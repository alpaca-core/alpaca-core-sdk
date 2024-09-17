// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
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

typedef struct ac_model_desc_asset {
    const char* path;
    const char* tag;
} ac_model_desc_asset;

/**
 * @brief Create a model asynchronously
 *
 * This function initiates the asynchronous creation of a model. The result is returned
 * via the provided callback function.
 *
 * @param p Pointer to the API provider
 * @param inference_type Inference type of the model
 * @param assets Array of asset descriptions
 * @param assets_count Number of elements in the assets array
 * @param params Dictionary containing model parameters (can be NULL for default parameters)
 * @param result_cb Callback function to be called with the result
 * @param progress_cb Callback function for progress updates (can be NULL if not needed)
 * @param cb_user_data User data to be passed to the callbacks
 */
AC_LOCAL_EXPORT void ac_create_model(
    ac_local_provider* p,
    const char* inference_type,
    ac_model_desc_asset* assets,
    size_t assets_count,
    ac_dict_root* params,
    void (*result_cb)(ac_model* m, const char* error, void* user_data),
    void (*progress_cb)(ac_sv tag, float progress, void* user_data),
    void* cb_user_data
);

#if defined(__cplusplus)
}
#endif
