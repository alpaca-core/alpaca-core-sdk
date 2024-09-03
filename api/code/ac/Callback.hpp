// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Error.hpp"
#include <itlib/expected.hpp>
#include <functional>
#include <string_view>

namespace ac {

/**
 * @brief A type alias for the result of a callback operation.
 * 
 * @tparam R The type of the successful result.
 */
template <typename R>
using CallbackResult = itlib::expected<R, Error>;

/**
 * @brief A structure representing a callback with result and progress functions.
 * 
 * This structure is used to handle asynchronous operations in the Alpaca Core library.
 * It provides callbacks for both the final result and intermediate progress updates.
 * 
 * @tparam R The type of the result.
 * @tparam S The type of the progress value (default is Dict for stream data).
 *
 * Example usage:
 * @code
 * Callback<ac::ModelPtr> cb{
 *     [&](ac::CallbackResult<ac::ModelPtr> result) {
 *         if (result.has_error()) {
 *             std::cout << "Error: " << result.error().text << "\n";
 *             return;
 *         }
 *         auto model = std::move(result.value());
 *         // Use the model
 *     },
 *     [](float progress) {
 *         std::cout << "Loading progress: " << progress * 100 << "%\n";
 *     }
 * };
 * @endcode
 */
template <typename R, typename S = Dict>
struct Callback {
    /**
     * @brief Type alias for the result callback function.
     *
     * This function is called when the asynchronous operation completes,
     * either with a successful result or an error.
     */
    using ResultCb = std::function<void(CallbackResult<R>)>;

    /**
     * @brief The result callback function.
     */
    ResultCb resultCb;
    using StreamCb = std::function<void(std::string_view tag, S)>;
    StreamCb progressCb;
};

}
