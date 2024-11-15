// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ModelLoaderRegistry.hpp"
#include <astl/throw_stdex.hpp>
#include <astl/move.hpp>

namespace ac::local {

const ModelLoaderInfo* ModelLoaderRegistry::findLoader(std::string_view schemaType) const {
    for (auto& loader : loaders) {
        if (loader.schemaType == schemaType) {
            return &loader;
        }
    }
    return nullptr;
}

ModelPtr ModelLoaderRegistry::createModel(ModelDesc desc, Dict params, ProgressCb cb) {
    auto l = findLoader(desc.inferenceType);
    if (!l) {
        ac::throw_ex{} << "No loader found for schema type: " << desc.inferenceType;
    }
    assert(l->loader);
    return l->loader->loadModel(astl::move(desc), astl::move(params), astl::move(cb));
}

} // namespace ac::local
