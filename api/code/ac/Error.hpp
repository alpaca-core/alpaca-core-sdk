// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <cstdint>
#include <string>

namespace ac {

/**
 * @brief A structure representing an error in the Alpaca Core library.
 *
 * This structure is used to encapsulate error information throughout the library.
 */
struct Error {
    /**
     * @brief A string containing the error message or description.
     */
    std::string text;
};

} // namespace ac
