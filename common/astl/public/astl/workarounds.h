// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

// workaroudns for various compiler bugs

#if defined(_MSC_VER)

// workaround for msvc bug where it produces C4715 (not all control paths return a value)
// even though noreturn destructor is invoked (but only sometimes)
// https://developercommunity.visualstudio.com/t/noreturn-destructor-may-incorrectly/10766806
// we add this macro after the noreturning dtor until we can remove it
#define MSVC_WO_10766806() __assume(0)

#else

#define MSVC_WO_10766806()

#endif
