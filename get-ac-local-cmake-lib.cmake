# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

# cpm
if(NOT CPM_SOURCE_CACHE AND NOT DEFINED ENV{CPM_SOURCE_CACHE})
    set(CPM_SOURCE_CACHE "${CMAKE_SOURCE_DIR}/.cpm")
    message(STATUS "Setting cpm cache dir to: ${CPM_SOURCE_CACHE}")
endif()
include(./get_cpm.cmake)

# ac-local-cmake-lib
if(AC_MONO_ROOT OR AC_MONO_COMP)
    set(acLocalCMakeLibDir "${CMAKE_SOURCE_DIR}/../ac-local-cmake-lib")
else()
    CPMAddPackage(gh:alpaca-core/ac-local-cmake-lib@1.0.0)
    set(acLocalCMakeLibDir "${ac-local-cmake-lib_SOURCE_DIR}")
endif()

list(APPEND CMAKE_MODULE_PATH
    "${acLocalCMakeLibDir}"
)
