// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ModelLoaderScorer.hpp"
#include "ModelLoader.hpp"
#include "Model.hpp"
#include "Instance.hpp"
#include "VectorStore.hpp"

// export vtables for classes which only have that

namespace ac::local {
ModelLoaderScorer::~ModelLoaderScorer() = default;
ModelLoader::~ModelLoader() = default;
Model::~Model() = default;
Instance::~Instance() = default;
VectorStore::~VectorStore() = default;
} // namespace ac::local
