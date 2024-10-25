# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_auto_option(AC_JAVA_WRAPPER "${CMAKE_PROJECT_NAME}: build java wrapper library" ${AC_WRAPPER_DEFAULT_VALUE})

if(NOT AC_JAVA_WRAPPER)
    return()
endif()

if(ANDROID)
    # find_package(JNI) does not work on Android with earlier versions of CMake
    cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
endif()

find_package(JNI ${AC_JAVA_WRAPPER_REQUIRED})
if(NOT JNI_FOUND)
    message(STATUS "Skpping Java wrapper library")
    return()
endif()

if(sanitizerFlags)
    # TODO: #134
    message(WARNING "${CMAKE_PROJECT_NAME} is configured with '${sanitizerFlags}'. Java will likely fail to load the JNI library")
endif()

add_subdirectory(jni.hpp)
add_subdirectory(ac-jni)

# only buld java with CMake if we are the root project
# otherwise we're a subproject (likely of a gradle build and the java build is handled there)
if(alpacaCoreIsRoot)
    find_package(Java REQUIRED)
    include(UseJava)

    add_subdirectory(src)

    if(AC_BUILD_TESTS OR AC_BUILD_EXAMPLES)
        if(Java_VERSION_MAJOR LESS 17)
            message(SEND_ERROR "${CMAKE_PROJECT_NAME}: Java 17+ is required to build Java tests and examples")
        endif()
    endif()

    ac_add_test_subdir()
    ac_add_example_subdir()
endif()
