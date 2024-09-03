// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <memory>

namespace ac {

class Model;

/**
 * @brief A shared pointer type for Model objects.
 *
 * ModelPtr is a type alias for std::shared_ptr<Model>, providing a convenient
 * way to manage Model objects with shared ownership semantics. This allows
 * multiple parts of the code to share ownership of a Model instance, with
 * automatic memory management when the last reference is destroyed.
 *
 * Usage example:
 * @code
 * ac::ModelPtr model = std::make_shared<ConcreteModel>();
 * // Use model as needed
 * @endcode
 */
using ModelPtr = std::shared_ptr<Model>;

} // namespace ac
