// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

/**
 * @file api_provider.h
 * @brief Defines the API provider structure and functions for Alpaca Core.
 *
 * This file contains the declaration of the ac_api_provider structure and related functions,
 * which provide an abstraction layer for the Alpaca Core API. The API provider encapsulates
 * the implementation details of model creation and management, allowing users to interact
 * with various model types through a unified interface.
 */

/**
 * @typedef ac_api_provider
 * @brief Opaque structure representing an API provider.
 *
 * The ac_api_provider structure encapsulates the implementation details of the API provider.
 * Users interact with this structure through functions defined in the Alpaca Core API,
 * such as ac_create_model() and ac_free_api_provider().
 */
typedef struct ac_api_provider ac_api_provider;
