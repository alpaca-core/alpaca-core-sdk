// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api_provider.h"

/**
 * @file ApiCUtil.hpp
 * @brief Utilities for the C API provider interface.
 *
 * This file contains utility functions for converting between C and C++
 * representations of the API provider.
 */

namespace ac {
class Provider;

/**
 * @namespace ac::cutil
 * @brief Namespace for C API utility functions.
 */
namespace cutil {

/**
 * @brief Converts a C API provider to a C++ Provider object.
 * @param p Pointer to the C API provider.
 * @return Pointer to the corresponding C++ Provider object.
 */
AC_API_EXPORT Provider* Provider_from_provider(ac_api_provider* p);

/**
 * @brief Converts a C++ Provider object to a C API provider.
 * @param p Pointer to the C++ Provider object.
 * @return Pointer to the corresponding C API provider.
 */
AC_API_EXPORT ac_api_provider* Provider_to_provider(Provider* p);

} // namespace cutil
} // namespace ac
