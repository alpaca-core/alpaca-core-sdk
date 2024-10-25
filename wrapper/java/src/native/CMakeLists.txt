# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

# this is a root CMakelists.txt file
# don't mind that it's deep within the directory structure of the repo
# it's used to build the project through gradle as opposed to as a cmake root project

cmake_minimum_required(VERSION 3.24)

project(alpaca-core-java)

#################
# cpm
if(NOT CPM_SOURCE_CACHE AND NOT DEFINED ENV{CPM_SOURCE_CACHE})
    set(CPM_SOURCE_CACHE "${CMAKE_CURRENT_SOURCE_DIR}/.cpm")
    message(STATUS "Setting cpm cache dir to: ${CPM_SOURCE_CACHE}")
endif()

#################
# config
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#################
set(AC_JAVA_WRAPPER ON)
add_subdirectory(../../../.. alpaca-core)
