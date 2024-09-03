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
 * Example usage:
 * @code
 * ac::LocalProvider provider;
 * ac::addLocalLlamaInference(provider);
 * provider.addAssetSource(ac::AssetSourceLocalDir_Create(AC_TEST_DATA_LLAMA_DIR), 0);
 *
 * provider.addModel(ac::ModelInfo{
 *     .id = "gpt2",
 *     .inferenceType = "llama.cpp",
 *     .assets = {{"gpt2-117m-q6_k.gguf", {}}}
 * });
 * @endcode
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
     * @code
     * provider.createModel("gpt2", {}, {
     *     [&](ac::CallbackResult<ac::ModelPtr> result) {
     *         // Handle the result
     *     },
     *     [](float progress) {
     *         // Handle progress updates
     *     }
     * });
     * @endcode
     */
    virtual void createModel(std::string_view id, Dict params, Callback<ModelPtr> cb) = 0;

    /**
     * @brief Adds a model to the provider.
     *
     * This method should be implemented by derived classes to add a model to the provider's
     * available models.
     *
     * @param modelInfo The information about the model to be added.
     */
    virtual void addModel(const ModelInfo& modelInfo) = 0;

    /**
     * @brief Adds an asset source to the provider.
     *
     * This method should be implemented by derived classes to add an asset source to the provider.
     *
     * @param source The asset source to be added.
     * @param priority The priority of the asset source.
     */
    virtual void addAssetSource(AssetSourcePtr source, int priority) = 0;
};

}
