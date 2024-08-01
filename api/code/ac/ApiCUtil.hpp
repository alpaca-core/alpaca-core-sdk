// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api_provider.h"

// utilities for the c dict api

namespace ac {
class Provider;
namespace cutil {
AC_API_EXPORT Provider* Provider_from_provider(ac_api_provider* p);
AC_API_EXPORT ac_api_provider* Provider_to_provider(Provider* p);
}
}
