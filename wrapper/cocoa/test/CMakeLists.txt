# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(xctest_cmake_modules)
include(FindSwiftXCTest)

add_swift_xctest(acCocoaTests ac-cocoa TestDict.mm)
