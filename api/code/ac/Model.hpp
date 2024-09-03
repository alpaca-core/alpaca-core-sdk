// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "InstancePtr.hpp"
#include "Callback.hpp"
#include "Dict.hpp"

namespace ac {

/**
 * @brief Abstract base class for models in the Alpaca Core library.
 *
 * The Model class serves as an interface for creating instances of various
 * model types within the Alpaca Core framework. It provides a common API
 * for model instantiation and management.
 *
 * Example usage:
 * @code
 * provider.createModel("gpt2", {}, {
 *     [&](ac::CallbackResult<ac::ModelPtr> result) {
 *         if (result.has_error()) {
 *             std::cout << "model load error: " << result.error().text << "\n";
 *             return;
 *         }
 *         auto model = std::move(result.value());
 *         
 *         // Use the model to create an instance
 *         model->createInstance("general", {}, {
 *             [&](ac::CallbackResult<ac::InstancePtr> instanceResult) {
 *                 if (instanceResult.has_error()) {
 *                     std::cout << "instance create error: " << instanceResult.error().text << "\n";
 *                     return;
 *                 }
 *                 auto instance = std::move(instanceResult.value());
 *                 // Use the instance for inference
 *             },
 *             {} // empty progress callback
 *         });
 *     },
 *     [](float progress) {
 *         // Handle loading progress
 *     }
 * });
 * @endcode
 */
class AC_API_EXPORT Model {
public:
    /**
     * @brief Virtual destructor for proper cleanup of derived classes.
     */
    virtual ~Model();

    /**
     * @brief Creates an instance of a specific model type.
     *
     * This pure virtual function must be implemented by derived classes to
     * create instances of specific model types. The instance creation is
     * asynchronous, and the result is returned via the provided callback.
     *
     * @param type The type of model instance to create (e.g., "general").
     * @param params A dictionary of parameters for model instantiation.
     * @param cb A callback function to be invoked with the created InstancePtr and optional progress updates.
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
     *         // Use the instance for inference
     *     },
     *     {} // empty progress callback
     * });
     * @endcode
     */
    virtual void createInstance(std::string_view type, Dict params, Callback<InstancePtr> cb) = 0;
};

} // namespace ac
