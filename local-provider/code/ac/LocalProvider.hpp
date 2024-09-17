// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "ModelDesc.hpp"
#include <ac/ModelPtr.hpp>
#include <ac/Callbacks.hpp>
#include <ac/Dict.hpp>
#include <string_view>
#include <memory>

// provider interface

namespace ac {

namespace asset {
class Source;
}

class LocalInferenceModelLoader;

class AC_LOCAL_EXPORT LocalProvider {
public:
    enum InitFlags : uint32_t {
        Default_Init = 0,

        // if the provider does not launch its own inference thread, it is the responsibility of the user to call
        // run and pushStop/abortRun appropriately - within the lifetime of the provider
        No_LaunchThread = 1,
    };

    explicit LocalProvider(uint32_t flags = Default_Init);
    ~LocalProvider();

    struct ModelCb {
        /**
         * @brief The result callback function.
         */
        ResultCb<ModelPtr> resultCb;

        /**
         * @brief The progress callback function.
         *
         * This function is called periodically to report progress during the asynchronous operation.
         * The use of tags in the callback allows for more flexible progress reporting,
         * especially in complex operations composed of multiple subtasks.
         */
        ProgressCb progressCb;
    };
    void createModel(ModelDesc desc, Dict params, ModelCb cb);

    void addLocalInferenceLoader(std::string_view type, LocalInferenceModelLoader& loader);

    //////////////////////////////////////////////////////////////////////////
    // no thread functions
    void run(); // block the current thread until stop is called
    void abortRun(); // stop the provider execution potentially aborting any in-flight operations
    void pushStop(); // stop the provider execution after all in-flight operations are completed
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}