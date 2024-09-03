/**
 * @file dict_ref.h
 * @brief Defines the reference type for dictionary operations in the Alpaca Core library.
 *
 * This file contains the definition of the opaque pointer type used as a reference
 * to dictionary objects in the C API of the Alpaca Core library. The dictionary
 * implementation is based on the nlohmann::json library, providing a flexible
 * and efficient way to handle JSON-like data structures.
 */

// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

/**
 * @typedef ac_dict_ref
 * @brief An opaque pointer type representing a reference to a dictionary object.
 *
 * This type is used throughout the C API to refer to dictionary objects without
 * exposing the internal implementation details. Functions in the API accept and
 * return this type when operating on dictionaries.
 *
 * Internally, ac_dict_ref corresponds to a pointer to the C++ ac::Dict type,
 * which is an alias for nlohmann::json. This allows the C API to work with
 * the underlying C++ implementation while maintaining type safety and
 * encapsulation.
 *
 * The dictionary can hold various types of data, including:
 * - Null values
 * - Booleans
 * - Numbers (integers, unsigned integers, and floating-point)
 * - Strings
 * - Arrays
 * - Objects (key-value pairs)
 * - Binary data
 *
 * Use the provided C API functions to create, manipulate, and query dictionary
 * objects through this reference type.
 */
typedef struct tag_ac_dict_ref* ac_dict_ref;
