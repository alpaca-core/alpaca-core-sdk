# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_auto_option(AC_COCOA_WRAPPER "${CMAKE_PROJECT_NAME}: build COCOA wrapper library" AUTO)

if(NOT AC_COCOA_WRAPPER)
    return()
endif()

if(NOT APPLE)
    message(STATUS "It's not Apple platform. Skpping COCOA wrapper library")
    return()
endif()

add_subdirectory(code)
ac_add_test_subdir()
