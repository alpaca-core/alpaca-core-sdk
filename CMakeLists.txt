# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(ac-local
    VERSION 0.1.1
    DESCRIPTION "Alpaca Core Local SDK"
    LANGUAGES C CXX
)

set(PROJECT_VERSION_TAG dev)

include(./get-ac-build.cmake)

#################
# cmake lib
CPMAddPackage(gh:iboB/icm@1.5.4)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
    "${icm_SOURCE_DIR}"
)

include(init_ac_prj)

include(icm_add_lib)
include(ac_local_lib)
include(ac_lib_subdirs)
include(icm_testing)
include(icm_build_failure_testing)
include(ac_dep)
include(icm_option)
include(ac_module)
include(ac_local_plugin_util)

#################
# cfg
if(NOT PROJECT_IS_TOP_LEVEL)
    # make our version known to projects which add us as a subdirectory
    set(ac-local_VERSION ${ac-local_VERSION} PARENT_SCOPE)
endif()

option(BUILD_SHARED_LIBS "${PROJECT_NAME}: build shared libraries" ON)

option(AC_BUILD_LOCAL "${PROJECT_NAME}: build local inference api" ON)

set(AC_INFERENCE_LIBS "all" CACHE STRING "${PROJECT_NAME}: inference libraries to build")

option(AC_BUILD_TOOLS "${PROJECT_NAME}: build tools" ON)

option(AC_LOCAL_BUILD_DUMMY_PLUGIN "${PROJECT_NAME}: build dummy plugin for tests and examples" ON)

option(AC_BUILD_TESTS "${PROJECT_NAME}: build tests" ${testsDefault})
option(AC_BUILD_EXAMPLES "${PROJECT_NAME}: build examples" ${examplesDefault})
option(AC_BUILD_POC "${PROJECT_NAME}: build proof of concept targets" ${examplesDefault})
mark_as_advanced(AC_BUILD_TESTS AC_BUILD_EXAMPLES AC_BUILD_POC)

#################
# global packages

CPMAddPackage(gh:iboB/itlib@1.11.4)

if(AC_BUILD_TESTS)
    CPMAddPackage(gh:iboB/doctest-util@0.1.3)
    set_target_properties(doctest PROPERTIES FOLDER test)
    set_target_properties(doctest-main PROPERTIES FOLDER test)
    enable_testing()
endif()

#################
# subdirs/targets

add_subdirectory(common)
add_subdirectory(dict)

ac_add_local_subdir()

if(AC_LOCAL_BUILD_DUMMY_PLUGIN)
    add_subdirectory(dummy-plugin)
endif()

if(NOT AC_INFERENCE_LIBS STREQUAL "none")
    #add_subdirectory(inference)
endif()

if(AC_BUILD_ACORD)
    # add_subdirectory(acord)
endif()

if(AC_BUILD_LOCAL)
    # add after inference libs so that elements from them can be wrapped as well
    #add_subdirectory(wrapper)
endif()

if(AC_BUILD_POC)
    add_subdirectory(poc)
endif()

#################
# export and install
configure_file(
    ac-local-config-version.in.cmake
    ac-local-config-version.cmake
    @ONLY
)

# export
export(EXPORT ac-local-targets)
configure_file(
    ac-local-config-export.in.cmake
    ac-local-config.cmake
    @ONLY
)

# install
install(EXPORT ac-local-targets
    DESTINATION share/cmake
)
