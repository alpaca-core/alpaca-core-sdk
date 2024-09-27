// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "ModelPtr.hpp"
#include "ModelDesc.hpp"
#include "ProgressCb.hpp"
#include <ac/Dict.hpp>

namespace ac::local {

/**
 * @brief Abstract base class for loading models.
 *
 * The ModelLoader class provides an interface to for loading models.
 *
 * Model loaders can be registered by interence type in @ref ModelFactory.
 */
class AC_LOCAL_EXPORT ModelLoader {
public:
    /**
     * @brief Virtual destructor for ModelLoader.
     */
    virtual ~ModelLoader();

    /**
     * @brief Loads a model based on the provided description and parameters.
     *
     * This function is responsible for synchronously loading a model using the given description and parameters.
     * It also accepts a progress callback function to report the loading progress. It's typically invoked by the
     * @ref ModelFactory after selecting the appropriate loader based on the model description.
     *
     * @param desc The description of the model to be loaded.
     * @param params A dictionary of parameters required for loading the model.
     * @param cb A callback function to report the progress of the model loading.
     *
     * @return A shared pointer to the loaded model.
     */
    virtual ModelPtr loadModel(ModelDesc desc, Dict params, ProgressCb cb) = 0;
};

} // namespace ac::local
