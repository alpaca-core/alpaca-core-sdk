// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <memory>

namespace ac::local {

class Model;

/**
 * @brief A shared pointer type for Model objects.
 *
 * ModelPtr is a type alias for std::shared_ptr<Model>, providing a convenient
 * way to manage Model objects with shared ownership semantics. This allows
 * multiple parts of the code to share ownership of a Model instance, with
 * automatic memory management when the last reference is destroyed.
 */
using ModelPtr = std::shared_ptr<Model>;

} // namespace ac::local
