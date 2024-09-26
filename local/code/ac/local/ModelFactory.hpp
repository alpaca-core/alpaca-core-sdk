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

class AC_LOCAL_EXPORT ModelFactory {
public:
    ModelFactory() = default;
    ModelFactory(const ModelFactory&) = delete;
    ModelFactory& operator=(const ModelFactory&) = delete;

    ModelPtr createModel(ModelDesc desc, Dict params, ProgressCb cb = {});

    void addLoader(std::string_view type, ModelLoader& loader);
    void removeLoader(std::string_view type);
private:
    // should be of reference_wrapper, but it's too much hassle
    astl::tsumap<ModelLoader*> m_loaders;
};

} // namespace ac::local
