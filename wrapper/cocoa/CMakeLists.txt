# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_auto_option(AC_COCOA_WRAPPER "${CMAKE_PROJECT_NAME}: build Cocoa wrapper library" AUTO)

if(NOT AC_COCOA_WRAPPER)
    return()
endif()

if(NOT APPLE)
    if(AC_COCOA_WRAPPER_REQUIRED)
        message(SEND_ERROR "Can't configure required Cocoa wrapper on a non-Apple platform")
    endif()
    return()
endif()

add_subdirectory(code)
ac_add_test_subdir()
