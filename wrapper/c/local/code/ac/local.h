// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"

#include <ac/dict_ref.h>
#include <ac/dict_arg.h>
#include <ac/sv.h>

#if defined(__cplusplus)
extern "C" {
#endif

/// @defgroup c-local Local inference API
/// C API for local inference.

/// @addtogroup c-local
/// @{

/// Get the last local inference error message or `NULL` if no error.
/// The function returns a thread-local string. Ownership of the string is not transferred.
/// @note Every `local_` function invalidates the last error.
AC_C_LOCAL_EXPORT const char* ac_local_get_last_error();

/// Opaque structure representing an instance.
typedef struct ac_local_instance ac_local_instance;

/// Free a local instance.
/// `NULL` is a no-op.
AC_C_LOCAL_EXPORT void ac_free_local_instance(ac_local_instance* i);

/// Run an op on a local instance
/// `target` is where the result of the operation is stored.
/// @return `target` or `NULL` on error.
AC_C_LOCAL_EXPORT ac_dict_ref ac_run_local_op(
    ac_dict_ref target,
    ac_local_instance* i,
    const char* op,
    ac_dict_arg params,
    bool (*progress_cb)(ac_sv tag, float progress, void* user_data),
    void* cb_user_data
);

/// Opaque structure representing a model.
typedef struct ac_local_model ac_local_model;

/// Free a local model.
/// `NULL` is a no-op.
AC_C_LOCAL_EXPORT void ac_free_local_model(ac_local_model* m);

/// Create a local instance of a model.
/// Returns `NULL` on error.
AC_C_LOCAL_EXPORT ac_local_instance* ac_create_local_instance(
    ac_local_model* m,
    const char* instance_type,
    ac_dict_arg params
);

/// Opaque structure representing a model factory.
typedef struct ac_local_model_factory ac_local_model_factory;

/// Create a new local model factory.
AC_C_LOCAL_EXPORT ac_local_model_factory* ac_new_local_model_factory();

/// Free a local model factory.
/// `NULL` is a no-op.
AC_C_LOCAL_EXPORT void ac_free_local_model_factory(ac_local_model_factory* f);

/// Asset info for creating a local model.
typedef struct ac_local_model_desc_asset {
    const char* path; ///< Path to the asset.
    const char* tag;  ///< Tag of the asset (may be `NULL`).
} ac_local_model_desc_asset;

/// Create a local model.
/// Returns `NULL` on error.
AC_C_LOCAL_EXPORT ac_local_model* ac_create_local_model(
    ac_local_model_factory* f,
    const char* inference_type,
    ac_local_model_desc_asset* assets,
    size_t assets_count,
    ac_dict_arg params,
    bool (*progress_cb)(ac_sv tag, float progress, void* user_data),
    void* cb_user_data
);

/// @}

#if defined(__cplusplus)
} // extern "C"

namespace ac::local {
class ModelFactory;
namespace cutil {
AC_C_LOCAL_EXPORT ModelFactory& ModelFactory_toCpp(ac_local_model_factory* f);
} // namespace cutil
} // namespace ac::local

#endif
