// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "ModelPtr.hpp"
#include "ModelDesc.hpp"
#include "ProgressCb.hpp"
#include <ac/Dict.hpp>
#include <astl/tsumap.hpp>

namespace ac::local {
class ModelLoader;

/**
 * @brief Factory class for creating models.
 *
 * The ModelFactory class is responsible for creating models based on a given description and parameters.
 * It uses registered loaders to perform the actual model loading.
 */
class AC_LOCAL_EXPORT ModelFactory {
public:
    ModelFactory() = default; ///< Default constructor.
    ModelFactory(const ModelFactory&) = delete; ///< Deleted copy constructor.
    ModelFactory& operator=(const ModelFactory&) = delete; ///< Deleted copy assignment operator.

    /**
     * @brief Creates a model based on the provided description and parameters.
     *
     * This function uses the appropriate loader to create a model based on the given description and parameters.
     * It also accepts a progress callback function to report the loading progress.
     *
     * @param desc The description of the model to be created.
     * @param params A dictionary of parameters required for creating the model.
     * @param cb An optional callback function to report the progress of the model creation.
     *
     * @return A shared pointer to the created model.
     *
     * @throws std::runtime_error If no suitable loader is found for the given model type.
     */
    ModelPtr createModel(ModelDesc desc, Dict params, ProgressCb cb = {});

    /**
     * @brief Registers a model loader for a given inference type.
     *
     * This function registers a model loader for a given inference type.
     *
     * @param type The inference type of the model that the loader can load.
     * @param loader A reference to the model loader to be registered.
     *
     * @throws std::runtime_error If a loader is already registered for the given inference type.
     *
     * @note The loader must remain valid for the lifetime of the ModelFactory object.
     */
    void addLoader(std::string_view type, ModelLoader& loader);

    /**
     * @brief Removes the model loader for a given inference type.
     *
     * This function removes the model loader for a given inference type.
     *
     * @param type The inference type of the model whose loader should be removed.
     *
     * @throws std::runtime_error If no loader is registered for the given inference type.
     */
    void removeLoader(std::string_view type);
private:
    // should be of reference_wrapper, but it's too much hassle
    astl::tsumap<ModelLoader*> m_loaders;
};

} // namespace ac::local
