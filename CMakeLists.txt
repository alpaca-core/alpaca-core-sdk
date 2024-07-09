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
CPMAddPackage(gh:iboB/icm@1.5.0)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
    "${icm_SOURCE_DIR}"
)

include(icm_add_lib)

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

#################
# global packages
CPMAddPackage(gh:iboB/splat@1.3.2)
CPMAddPackage(gh:nlohmann/json@3.11.3)
CPMAddPackage(gh:iboB/itlib@1.10.3)

#################
# subdirs/targets
add_subdirectory(api)
add_subdirectory(poc)
