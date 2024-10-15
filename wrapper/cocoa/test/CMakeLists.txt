# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

include(FindSwiftXCTest)

add_swift_xctest(acCocoaTests ac-cocoa TestDict.mm)
target_include_directories(acCocoaTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../code/ac
)
