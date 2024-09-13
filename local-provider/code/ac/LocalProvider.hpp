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

namespace asset {
class Source;
}

class LocalInferenceModelLoader;
struct ModelInfo;

class AC_LOCAL_EXPORT LocalProvider final : public Provider {
public:
    enum InitFlags : uint32_t {
        Default_Init = 0,

        // if the provider does not launch it's own threads, it is the responsibility of the user to call
        // runInference, runAssetManagement and abortThreads appropriately - within the lifetime of the provider
        No_LaunchThreads = 1,
    };

    explicit LocalProvider(uint32_t flags = Default_Init);
    ~LocalProvider();
    virtual void createModel(std::string_view id, Dict params, Callback<ModelPtr> cb) override;

    void addAssetSource(std::unique_ptr<asset::Source> source, int priority);
    void addModel(ModelInfo info);
    void addLocalInferenceLoader(std::string_view type, LocalInferenceModelLoader& loader);
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}