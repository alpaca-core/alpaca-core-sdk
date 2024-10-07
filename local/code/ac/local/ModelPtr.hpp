// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <memory>

namespace ac::local {

class Model;

/// A shared pointer type for Model objects.
/// Ideally this would a local shared pointer, but this on its own doesn't warrant including boost as a dependency.
/// The SDK does not make use of this shared pointer, but it provides it for convenience in case the user needs it.
using ModelPtr = std::shared_ptr<Model>;

} // namespace ac::local
