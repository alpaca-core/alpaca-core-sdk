# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_auto_option(AC_SWIFT_WRAPPER "${CMAKE_PROJECT_NAME}: build Swift wrapper library" ${AC_WRAPPER_DEFAULT_VALUE})

if(NOT AC_SWIFT_WRAPPER)
    return()
endif()

if(sanitizerFlags)
    # TODO: #159
    message(${AC_SWIFT_WRAPPER_MSG_STATUS} "${CMAKE_PROJECT_NAME}: Can't configure Swift wrapper. \
        The project is configured with '${sanitizerFlags}'. swiftc cannot does not understand them. ")
    return()
endif()

if(NOT ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    # https://www.swift.org/documentation/cxx-interop/status/#compiler-support
    message(${AC_SWIFT_WRAPPER_MSG_STATUS} "${CMAKE_PROJECT_NAME}: Can't configure Swift wrapper. "
        "C++ compiler is not clang and Swift/C++ interop only works with clang.")
    return()
endif()

find_program(SWIFT_COMPILER swiftc ${AC_SWIFT_WRAPPER_REQUIRED})
if(NOT SWIFT_COMPILER)
    message(STATUS "${CMAKE_PROJECT_NAME}: Can't configure Swift wrapper. Swift compiler not found.")
    return()
endif()

if(POLICY CMP0157)
	# enable proper Swift incremental building with ninja
	# this is the default for cmake 3.29, but the issue doesn't warrant bumping min required
	cmake_policy(SET CMP0157 NEW)
endif()
enable_language(Swift)

if(CMAKE_Swift_COMPILER_VERSION VERSION_LESS "6.0")
    message(FATAL_ERROR "Swift version must be at least 6.0. Found: ${CMAKE_Swift_COMPILER_VERSION}")
else()
    message(STATUS "Swift version is sufficient: ${CMAKE_Swift_COMPILER_VERSION}")
endif()

add_subdirectory(code)
ac_add_example_subdir()
ac_add_test_subdir()
