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

/// Base class for model loaders.
/// Model loaders are responsible for loading models based on the provided description and parameters. They are typically
/// facades for an underlying inference library. While model loaders can be used on their own, they are typically used
/// via the `ModelFactory` class.
/// @ingroup cpp-local
class AC_LOCAL_EXPORT ModelLoader {
public:
    virtual ~ModelLoader();

    /// Load a model based on the provided description and parameters.
    /// The progress callback is optional and can be used to report the progress of the loading process.
    /// The returned model is owned by the caller and is not bound to the loader in any way.
    virtual ModelPtr loadModel(ModelDesc desc, Dict params, ProgressCb cb) = 0;
};

} // namespace ac::local
