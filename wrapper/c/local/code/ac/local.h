// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <splat/symbol_export.h>

#include <ac/dict_ref.h>
#include <ac/dict_root.h>
#include <ac/sv.h>

#if AC_C_LOCAL_SHARED
#   if BUILDING_AC_C_LOCAL
#       define AC_C_LOCAL_EXPORT SYMBOL_EXPORT
#   else
#       define AC_C_LOCAL_EXPORT SYMBOL_IMPORT
#   endif
#else
#   define AC_C_LOCAL_EXPORT
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Get the last error message.
 *
 * @return const char* Null if no error, otherwise a thread-local error string.
 * @note Every function invalidates the previous error.
 */
AC_C_LOCAL_EXPORT const char* ac_local_get_last_error();

/**
 * @brief Opaque structure representing an instance
 */
typedef struct ac_local_instance ac_local_instance;

/**
 * @brief Free an instance
 *
 * This function should be called to clean up the instance object when it's no longer needed.
 *
 * @param i Pointer to the instance to be freed
 */
AC_C_LOCAL_EXPORT void ac_free_local_instance(ac_local_instance* i);

/**
 * @brief Run an operation on an instance
 *
 * This function initiates an operation on the given instance.
 * The result and intermediate updates are returned via the provided callback functions.
 *
 * @param i Pointer to the instance
 * @param op Name of the operation to run (e.g., "run")
 * @param params_root Dictionary containing operation parameters
 * @param progress_cb Optional callback function for non-result-related progress
 * @param cb_user_data User data to be passed to the callbacks
 * @return Dictionary containing the result of the operation
 */
AC_C_LOCAL_EXPORT ac_dict_root* ac_run_local_op(
    ac_local_instance* i,
    const char* op,
    ac_dict_root* params_root,
    bool (*progress_cb)(ac_sv tag, float progress, void* user_data),
    void* cb_user_data
);

AC_C_LOCAL_EXPORT bool ac_have_local_stream(ac_local_instance* i);
AC_C_LOCAL_EXPORT bool ac_push_local_stream(ac_local_instance* i, ac_dict_root* params_root);
AC_C_LOCAL_EXPORT ac_dict_root* ac_pull_local_stream(ac_local_instance* i);

/**
 * @brief Opaque structure representing a model
 */
typedef struct ac_local_model ac_local_model;

AC_C_LOCAL_EXPORT void ac_free_local_model(ac_local_model* m);

/**
 * @brief Create an instance asynchronously
 *
 * This function initiates the asynchronous creation of an instance from a model.
 * The result is returned via the provided callback function.
 *
 * @param m Pointer to the model
 * @param instance_type Type of instance to be created (e.g., "general")
 * @param params_root Dictionary containing instance parameters (can be NULL for default parameters)
 * @param result_cb Callback function to be called with the result
 * @param cb_user_data User data to be passed to the callback
 */
AC_C_LOCAL_EXPORT ac_local_instance* ac_create_local_instance(
    ac_local_model* m,
    const char* instance_type,
    ac_dict_root* params_root
);

typedef struct ac_local_model_factory ac_local_model_factory;
AC_C_LOCAL_EXPORT ac_local_model_factory* ac_new_local_model_factory();
AC_C_LOCAL_EXPORT void ac_free_local_model_factory(ac_local_model_factory* f);

typedef struct ac_local_model_desc_asset {
    const char* path;
    const char* tag;
} ac_local_model_desc_asset;

AC_C_LOCAL_EXPORT ac_local_model* ac_create_local_model(
    ac_local_model_factory* f,
    const char* inference_type,
    ac_local_model_desc_asset* assets,
    size_t assets_count,
    ac_dict_root* params_root,
    bool (*progress_cb)(ac_sv tag, float progress, void* user_data),
    void* cb_user_data
);

#if defined(__cplusplus)
} // extern "C"

namespace ac::local {
class ModelFactory;
namespace cutil {
AC_C_LOCAL_EXPORT ModelFactory& ModelFactory_toCpp(ac_local_model_factory* f);
} // namespace cutil
} // namespace ac::local

#endif
