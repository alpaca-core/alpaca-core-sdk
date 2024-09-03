// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Callback.hpp"

namespace ac {

/**
 * @class Instance
 * @brief Abstract base class representing an instance of a model or process.
 *
 * The Instance class provides an interface for running operations, synchronizing,
 * and aborting the instance's execution. It is typically created using a Model object.
 *
 * Example usage:
 * @code
 * model->createInstance("general", {}, {
 *     [&](ac::CallbackResult<ac::InstancePtr> result) {
 *         if (result.has_error()) {
 *             std::cout << "instance create error: " << result.error().text << "\n";
 *             return;
 *         }
 *         auto instance = std::move(result.value());
 *         
 *         // Use the instance to run operations
 *         instance->runOp("run", {{"prompt", prompt}, {"max_tokens", 20}, {"antiprompts", antiprompts}}, {
 *             [&](ac::CallbackResult<void> opResult) {
 *                 if (opResult.has_error()) {
 *                     std::cout << "run error: " << opResult.error().text << "\n";
 *                     return;
 *                 }
 *                 // Operation completed successfully
 *             },
 *             [](ac::Dict result) {
 *                 // Handle intermediate results
 *                 std::cout << result.at("result").get<std::string_view>();
 *             }
 *         });
 *     },
 *     {} // empty progress callback
 * });
 * @endcode
 */
class AC_API_EXPORT Instance {
public:
    /**
     * @brief Virtual destructor for the Instance class.
     */
    virtual ~Instance();

    /**
     * @brief Run an operation on the instance.
     *
     * Operations are queued and executed in order. The inner state is modified by ops,
     * and subsequent ones may depend on the results of previous ones.
     *
     * @param op The operation to run, specified as a string_view (e.g., "run").
     * @param params Parameters for the operation, provided as a Dict (e.g., prompt, max_tokens, antiprompts).
     * @param cb Callback function to be called with the operation's result and intermediate updates.
     *
     * Example usage:
     * @code
     * instance->runOp("run", {{"prompt", prompt}, {"max_tokens", 20}, {"antiprompts", antiprompts}}, {
     *     [&](ac::CallbackResult<void> result) {
     *         if (result.has_error()) {
     *             std::cout << "run error: " << result.error().text << "\n";
     *             return;
     *         }
     *         // Operation completed successfully
     *     },
     *     [](ac::Dict result) {
     *         // Handle intermediate results
     *         std::cout << result.at("result").get<std::string_view>();
     *     }
     * });
     * @endcode
     */
    virtual void runOp(std::string_view op, Dict params, Callback<void, Dict> cb) = 0;

    /**
     * @brief Wait for all operations to finish.
     *
     * This function blocks until all queued operations have completed.
     * It can be used to ensure all operations are finished before proceeding.
     *
     * Example usage:
     * @code
     * instance->synchronize();
     * std::cout << "All operations completed.\n";
     * @endcode
     */
    virtual void synchronize() = 0;

    /**
     * @brief Initiate the abort process for the instance.
     *
     * This function starts the process of aborting the current operation.
     * Note that some callbacks may still be called after this function.
     * Either wait for pending callbacks or call synchronize after this to guarantee
     * that no more callbacks will be called.
     *
     * @param cb Callback function to be called when the abort process is initiated.
     *
     * Example usage:
     * @code
     * instance->initiateAbort([](ac::CallbackResult<void> result) {
     *     if (result.has_error()) {
     *         std::cout << "Abort error: " << result.error().text << "\n";
     *         return;
     *     }
     *     std::cout << "Abort initiated successfully.\n";
     * });
     * instance->synchronize(); // Wait for all operations to finish
     * @endcode
     */
    virtual void initiateAbort(Callback<void> cb) = 0;
};

} // namespace ac
