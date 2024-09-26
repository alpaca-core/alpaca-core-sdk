/**
 * @file dict_root.h
 * @brief Defines the root structure for dictionary operations in the Alpaca Core library.
 *
 * This file contains the definition of the opaque structure type used as the root
 * for dictionary objects in the C API of the Alpaca Core library. The dictionary
 * implementation is based on the nlohmann::json library, providing a flexible
 * and efficient way to handle JSON-like data structures.
 */

// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

/**
 * @typedef ac_dict_root
 * @brief An opaque structure type representing the root of a dictionary object.
 *
 * This type is used as the base for creating and managing dictionary objects
 * in the C API. Functions in the API that create or manipulate entire dictionaries
 * typically work with pointers to this type.
 *
 * Internally, ac_dict_root contains an instance of the C++ ac::Dict type,
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
 * objects through this root structure.
 */
typedef struct ac_dict_root ac_dict_root;
