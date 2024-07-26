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

class AC_LOCAL_EXPORT LocalProvider : public Provider {
public:
    LocalProvider();
    ~LocalProvider();
    virtual void createModel(Dict params, Callback<ModelPtr> cb) override;

    void addLocalInferenceLoader(std::string_view type, LocalInferenceModelLoader& loader);
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}