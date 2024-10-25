# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_auto_option(AC_SWIFT_WRAPPER "${CMAKE_PROJECT_NAME}: build Swift wrapper library" ${AC_WRAPPER_DEFAULT_VALUE})

if(NOT AC_SWIFT_WRAPPER)
    return()
endif()

find_program(SWIFT_COMPILER swiftc ${AC_SWIFT_WRAPPER_REQUIRED})
if(NOT SWIFT_COMPILER)
    message(STATUS "Swift compiler not found. Skipping Swift wrapper library")
    return()
endif()

enable_language(Swift)

add_subdirectory(code)
ac_add_example_subdir()
ac_add_test_subdir()
