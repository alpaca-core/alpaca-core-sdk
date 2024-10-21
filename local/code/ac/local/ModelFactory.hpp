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

/// @defgroup cpp-local C++ Local API
/// C++ API for local infernence.

namespace ac::local {
class ModelLoader;

/// Factory class for creating models.
/// Facilitates the creation (or loading) of models based on `ModelDesc::inferenceType`.
/// The factory uses registered instances of `ModelLoader` to create models.
/// @ingroup cpp-local
class AC_LOCAL_EXPORT ModelFactory {
public:
    ModelFactory() = default;
    ModelFactory(const ModelFactory&) = delete;
    ModelFactory& operator=(const ModelFactory&) = delete;

    /// Create a model based on a given description and parameters.
    /// Finds a `ModelLoader` registered for the inference type in `desc` and forwards the call to it.
    /// The progress callback is optional and can be used to report the progress of the loading process.
    /// The returned model is owned by the caller and is not bound to the factory (or loader) in any way.
    /// @throws std::runtime_error if no loader is registered for the inference type in `desc`.
    ModelPtr createModel(ModelDesc desc, Dict params, ProgressCb cb = {});

    /// Register a model loader for a given inference type.
    /// The same loader can be registered for multiple inference types.
    /// @note The loader must remain valid for the lifetime of the ModelFactory object.
    /// @throws std::runtime_error if a loader is already registered for the given inference type.
    void addLoader(std::string_view type, ModelLoader& loader);

    /// Remove the model loader for a given inference type.
    /// @throws std::runtime_error if no loader is registered for the given inference type.
    void removeLoader(std::string_view type);
private:
    // should be of reference_wrapper, but it's too much hassle
    astl::tsumap<ModelLoader*> m_loaders;
};

} // namespace ac::local
