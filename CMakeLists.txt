# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(alpaca-core
    LANGUAGES C CXX
)

#################
# cpm
if(NOT CPM_SOURCE_CACHE AND NOT DEFINED ENV{CPM_SOURCE_CACHE})
    set(CPM_SOURCE_CACHE "${CMAKE_CURRENT_SOURCE_DIR}/.cpm")
    message(STATUS "Setting cpm cache dir to: ${CPM_SOURCE_CACHE}")
endif()
include(./get_cpm.cmake)

#################
# cmake lib
CPMAddPackage(gh:iboB/icm@1.5.1)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
    "${icm_SOURCE_DIR}"
)

include(icm_add_lib)
include(ac_lib_subdirs)
include(icm_testing)
include(icm_build_failure_testing)

#################
# cfg
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(alpacaCoreIsRoot YES)
endif()

if(alpacaCoreIsRoot)
    # setup various compiler settings that would otherwise be expected to come
    # from the outside
    include(./root-setup.cmake)
endif()

option(AC_BUILD_TESTS "${CMAKE_PROJECT_NAME}: build tests" ${alpacaCoreIsRoot})
option(AC_BUILD_EXAMPLES "${CMAKE_PROJECT_NAME}: build examples" ${alpacaCoreIsRoot})

#################
# global packages
CPMAddPackage(gh:iboB/splat@1.3.3)
CPMAddPackage(gh:nlohmann/json@3.11.3)
CPMAddPackage(gh:iboB/itlib@1.11.1)
CPMAddPackage(gh:iboB/jalog@0.4.2)
CPMAddPackage(gh:iboB/xec@1.5.1)

if(AC_BUILD_TESTS)
    CPMAddPackage(gh:iboB/doctest-util@0.1.2)
    set_target_properties(doctest PROPERTIES FOLDER test)
    set_target_properties(doctest-main PROPERTIES FOLDER test)
    CPMAddPackage(gh:ThrowTheSwitch/Unity@2.6.0)
    set_target_properties(unity PROPERTIES FOLDER test)
    enable_testing()
endif()

# test data packages
CPMAddPackage(
    NAME ac-test-data-llama
    VERSION 1.0.0
    GIT_REPOSITORY https://huggingface.co/alpaca-core/ac-test-data-llama
    GIT_TAG e6860e89d869ea815a4107bce3bb41af9ad1c1f0
)

if (EXISTS tmp/ac-test-data-whisper)
    add_subdirectory(tmp/ac-test-data-whisper)
endif()

#################
# subdirs/targets

add_subdirectory(common)
add_subdirectory(api)
add_subdirectory(local-provider)
add_subdirectory(inference)
add_subdirectory(poc)
