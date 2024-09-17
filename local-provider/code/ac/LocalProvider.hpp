// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
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
struct ModelInfo;

class AC_LOCAL_EXPORT LocalProvider {
public:
    enum InitFlags : uint32_t {
        Default_Init = 0,

        // if the provider does not launch it's own threads, it is the responsibility of the user to call
        // runInference, runAssetManagement and abortWorkers appropriately - within the lifetime of the provider
        No_LaunchThreads = 1,
    };

    explicit LocalProvider(uint32_t flags = Default_Init);
    ~LocalProvider();

    /**
     * @brief A structure representing a callback with result and progress functions.
     *
     * This structure is used to handle asynchronous operations in the provider.
     * It provides callbacks for both the final result and intermediate progress updates.
     *
     * @tparam R The type of the result.
     *
     * Example usage:
     * @snippet inference/llama.cpp/local/example/e-local-llama.cpp Provider_createModel Usage Example
     */
    template <typename R>
    struct Callback {
        /**
         * @brief The result callback function.
         */
        ResultCb<R> resultCb;

        /**
         * @brief The progress callback function.
         *
         * This function is called periodically to report progress during the asynchronous operation.
         * The use of tags in the callback allows for more flexible progress reporting,
         * especially in complex operations composed of multiple subtasks.
         */
        ProgressCb progressCb;
    };
    void createModel(std::string_view id, Dict params, Callback<ModelPtr> cb);

    void addAssetSource(std::unique_ptr<asset::Source> source, int priority);
    void addModel(ModelInfo info);
    void addLocalInferenceLoader(std::string_view type, LocalInferenceModelLoader& loader);

    //////////////////////////////////////////////////////////////////////////
    // no thread functions
    void runInference(); // blocks the current thread until abortWorkers is called
    void runAssetManagement(); // blocks the current thread until abortWorkers is called
    void abortWorkers(); // stops the inference and asset workers potentially aborting any in-flight operations
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}