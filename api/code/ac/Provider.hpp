// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "ModelPtr.hpp"
#include "Callback.hpp"
#include "Dict.hpp"

namespace ac {

/**
 * @class Provider
 * @brief Abstract base class for model providers in the Alpaca Core library.
 *
 * The Provider class defines the interface for creating and managing models in the Alpaca Core library.
 * It is designed to be subclassed by specific model providers, such as LocalProvider, allowing for
 * different implementations of model creation and management.
 *
 */
class AC_API_EXPORT Provider {
public:
    /**
     * @brief Virtual destructor for the Provider class.
     */
    virtual ~Provider();

    /**
     * @brief Creates a model asynchronously.
     * 
     * This pure virtual function is to be implemented by derived classes to create
     * a model with the given ID and parameters. The created model is returned via
     * the provided callback.
     *
     * @param id The unique identifier for the model to be created.
     * @param params A dictionary of parameters for model creation.
     * @param cb A callback function to be called with the created ModelPtr and optional progress updates.
     *
     * Example usage:
     * @snippet inference/llama.cpp/local/example/e-local-llama.cpp Provider_createModel Usage Example
     */
    virtual void createModel(std::string_view id, Dict params, Callback<ModelPtr> cb) = 0;
};

}

