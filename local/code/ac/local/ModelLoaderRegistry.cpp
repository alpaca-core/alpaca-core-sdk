// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ModelLoaderRegistry.hpp"
#include "ModelLoader.hpp"
#include <astl/throw_stdex.hpp>
#include <astl/move.hpp>
#include <astl/qalgorithm.hpp>

namespace ac::local {

void ModelLoaderRegistry::addLoader(ModelLoader& loader, PluginInfo* plugin) {
    m_loaders.push_back({loader, plugin});
}

std::optional<ModelLoaderRegistry::LoaderData> ModelLoaderRegistry::findLoader(std::string_view schemaType) const noexcept {
    for (auto& data : m_loaders) {
        if (astl::pfind(data.loader.info().inferenceSchemaTypes, schemaType)) {
            return data;
        }
    }
    return std::nullopt;
}

ModelPtr ModelLoaderRegistry::createModel(ModelDesc desc, Dict params, ProgressCb cb) const {
    auto data = findLoader(desc.inferenceType);
    if (!data) {
        ac::throw_ex{} << "No loader found for schema type: " << desc.inferenceType;
    }
    return data->loader.loadModel(astl::move(desc), astl::move(params), astl::move(cb));
}

} // namespace ac::local
