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
     */
    virtual void runOp(std::string_view op, Dict params, Callback<void, Dict> cb) = 0;

    /**
     * @brief Wait for all operations to finish.
     *
     * This function blocks until all queued operations have completed.
     * It can be used to ensure all operations are finished before proceeding.
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
     */
    virtual void initiateAbort(Callback<void> cb) = 0;
};

} // namespace ac
