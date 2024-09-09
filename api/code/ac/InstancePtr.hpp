// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <memory>

namespace ac {

/**
 * @brief Forward declaration of the Instance class.
 */
class Instance;

/**
 * @brief A shared pointer type for the Instance class.
 *
 * InstancePtr provides a convenient way to manage the lifetime of Instance objects
 * using reference counting. It allows multiple owners to share ownership of an
 * Instance object, automatically deleting the object when the last owner releases it.
 *
 * @see Instance
 * @see std::shared_ptr
 */
using InstancePtr = std::shared_ptr<Instance>;

} // namespace ac
