// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ModelFactory.hpp"
#include "ModelLoader.hpp"
#include <astl/throw_stdex.hpp>

namespace ac::local {

ModelPtr ModelFactory::createModel(ModelDesc desc, Dict params, ProgressCb cb) {
    auto it = m_loaders.find(desc.inferenceType);
    if (it == m_loaders.end()) {
        throw_ex{} << "Unknown inference type: " << desc.inferenceType;
    }
    auto& loader = *it->second;
    return loader.loadModel(desc, params, std::move(cb));
}

void ModelFactory::addLoader(std::string_view type, ModelLoader& loader) {
    auto [_, inserted] = m_loaders.try_emplace(std::string(type), &loader); // C++26: remove string cast
    if (!inserted) {
        throw_ex{} << "Loader already exists: " << type;
    }
}

void ModelFactory::removeLoader(std::string_view type) {
    auto f = m_loaders.find(type);
    if (f == m_loaders.end()) {
        throw_ex{} << "Loader not found: " << type;
    }
    m_loaders.erase(f);
}

} // namespace ac::local
