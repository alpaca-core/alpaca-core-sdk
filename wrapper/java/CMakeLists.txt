# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_auto_option(AC_JAVA_WRAPPER "${CMAKE_PROJECT_NAME}: biuld java wrapper library" AUTO)

if(NOT AC_JAVA_WRAPPER)
    return()
endif()

find_package(JNI ${AC_JAVA_WRAPPER_REQUIRED})
if(NOT JNI_FOUND)
    message(STATUS "Could not find JNI. Skpping Java wrapper library")
    return()
endif()

find_package(Java REQUIRED)
include(UseJava)

add_subdirectory(code)
ac_add_test_subdir()
