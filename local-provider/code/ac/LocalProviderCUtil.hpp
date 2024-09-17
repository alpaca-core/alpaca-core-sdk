// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "local_provider.h"

namespace ac {
class LocalProvider;
namespace cutil {

inline ac::LocalProvider* LocalProvider_toCpp(ac_local_provider* p) {
    return reinterpret_cast<ac::LocalProvider*>(p);
}
inline ac_local_provider* LocalProvider_fromCpp(ac::LocalProvider* p) {
    return reinterpret_cast<ac_local_provider*>(p);
}

} // namespace cutil
} // namespace ac
