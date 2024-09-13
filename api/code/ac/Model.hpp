// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Callbacks.hpp"
#include "InstancePtr.hpp"
#include "Dict.hpp"

namespace ac {

/**
 * @brief Abstract base class for models in the Alpaca Core library.
 *
 * The Model class serves as an interface for creating instances of various
 * model types within the Alpaca Core framework.
 *
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
     * @param cb A callback function to be invoked with the created InstancePtr or an error.
     *
     * Example usage:
     * @snippet inference/llama.cpp/local/example/e-local-llama.cpp Model_createInstance Usage Example
     */
    virtual void createInstance(std::string_view type, Dict params, ResultCb<InstancePtr> cb) = 0;
};

} // namespace ac
