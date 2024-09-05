// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Provider.hpp"
#include "Model.hpp"
#include "Instance.hpp"

/**
 * @file VtableExports.cpp
 * @brief Exports vtables for abstract classes in the Alpaca Core library.
 *
 * This file ensures that the vtables for abstract classes are exported from the
 * library. It provides default implementations for the destructors of Provider,
 * Model, and Instance classes.
 */

// export vtables for classes which only have that
namespace ac {

/**
 * @brief Default destructor for the Provider class.
 *
 * This ensures that the vtable for the Provider class is exported from the library.
 */
Provider::~Provider() = default;

/**
 * @brief Default destructor for the Model class.
 *
 * This ensures that the vtable for the Model class is exported from the library.
 */
Model::~Model() = default;

/**
 * @brief Default destructor for the Instance class.
 *
 * This ensures that the vtable for the Instance class is exported from the library.
 */
Instance::~Instance() = default;

} // namespace ac
