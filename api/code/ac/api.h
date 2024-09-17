// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "dict_ref.h"
#include "dict_root.h"
#include "model.h"
#include <astl/sv.h>

/**
 * @file api.h
 * @brief Main C API header for Alpaca Core
 *
 * This file contains the C wrapper for the C++ API functions and structures for Alpaca Core.
 * It provides a C-compatible interface for the Alpaca Core library, allowing it to be used
 * from C programs or other languages that can interface with C.
 *
 * Key components:
 * - ac_model: Wraps the C++ ModelPtr
 * - ac_instance: Wraps the C++ InstancePtr
 *
 * @example inference/llama.cpp/local/example/e-local-llama.c
 */

//General rules:
// - The ownership of dict_root is transferred to the function
// - The result_cb will be called with an error message if there is an error (and the payload will be null)
// - progress_cb can be null

#if defined(__cplusplus)
extern "C" {
#endif

// to obtain a provider use a library which can create it

/**
 * @brief Free a model
 *
 * This function should be called to clean up the model object when it's no longer needed.
 *
 * @param m Pointer to the model to be freed
 */
AC_API_EXPORT void ac_free_model(ac_model* m);

/**
 * @brief Opaque structure representing an instance
 */
typedef struct ac_instance ac_instance;

/**
 * @brief Free an instance
 *
 * This function should be called to clean up the instance object when it's no longer needed.
 *
 * @param i Pointer to the instance to be freed
 */
AC_API_EXPORT void ac_free_instance(ac_instance* i);

/**
 * @brief Create an instance asynchronously
 *
 * This function initiates the asynchronous creation of an instance from a model.
 * The result is returned via the provided callback function.
 *
 * @param m Pointer to the model
 * @param instance_type Type of instance to be created (e.g., "general")
 * @param dict_root Dictionary containing instance parameters (can be NULL for default parameters)
 * @param result_cb Callback function to be called with the result
 * @param cb_user_data User data to be passed to the callback
 */
AC_API_EXPORT void ac_create_instance(
    ac_model* m,
    const char* instance_type,
    ac_dict_root* dict_root,
    void (*result_cb)(ac_instance* i, const char* error, void* user_data),
    void* cb_user_data
);

/**
 * @brief Run an operation on an instance asynchronously
 *
 * This function initiates an asynchronous operation on the given instance.
 * The result and intermediate updates are returned via the provided callback functions.
 *
 * @param i Pointer to the instance
 * @param op Name of the operation to run (e.g., "run")
 * @param dict_root Dictionary containing operation parameters
 * @param completion_cb Callback function to be called upon completion (error will be null if successful)
 * @param stream_cb Callback function for the result (can be called multiple times, depending on op)
 * @param progress_cb Callback function for non-result-related progress (can be NULL if not needed)
 * @param cb_user_data User data to be passed to the callbacks
 */
AC_API_EXPORT void ac_run_op(
    ac_instance* i,
    const char* op,
    ac_dict_root* dict_root,
    void (*completion_cb)(const char* error, void* user_data),
    void (*stream_cb)(ac_dict_ref dict, void* user_data),
    void (*progress_cb)(ac_sv tag, float progress, void* user_data),
    void* cb_user_data
);

/**
 * @brief Synchronize an instance
 *
 * This function blocks until all queued operations on the instance have completed.
 * It can be used to ensure all operations are finished before proceeding.
 *
 * @param i Pointer to the instance to synchronize
 */
AC_API_EXPORT void ac_synchronize_instance(ac_instance* i);

/**
 * @brief Initiate an abort operation on an instance asynchronously
 *
 * This function starts the process of aborting the current operation on the instance.
 * Note that some callbacks may still be called after this function.
 * Either wait for pending callbacks or call ac_synchronize_instance after this to guarantee
 * that no more callbacks will be called.
 *
 * @param i Pointer to the instance
 * @param done_cb Callback function to be called when the abort operation is complete (can be NULL)
 * @param cb_user_data User data to be passed to the callback function
 */
AC_API_EXPORT void ac_initiate_instance_abort(
    ac_instance* i,
    void (*done_cb)(void* user_data),
    void* cb_user_data
);

#if defined(__cplusplus)
}
#endif
