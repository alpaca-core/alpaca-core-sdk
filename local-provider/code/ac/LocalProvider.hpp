// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <ac/Provider.hpp>
#include <string_view>
#include <memory>

// provider interface

namespace ac {

class LocalInferenceModelLoader;

class AC_LOCAL_EXPORT LocalProvider final : public Provider {
public:
    LocalProvider();
    ~LocalProvider();
    virtual void createModel(std::string_view id, Dict params, Callback<ModelPtr> cb) override;

    void addLocalModel(std::string_view id, Dict baseParams);
    void addLocalInferenceLoader(std::string_view type, LocalInferenceModelLoader& loader);
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}