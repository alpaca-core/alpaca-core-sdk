# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(alpaca-core-sdk
    VERSION 0.1.15
    DESCRIPTION "Alpaca Core Local SDK"
    LANGUAGES C CXX
)

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
include(icm_testing)
include(icm_build_failure_testing)
include(icm_option)

include(ac_lib)
include(ac_lib_subdirs)

include(ac_local_plugin_util)
include(ac_dep)

#################
# cfg
if(NOT PROJECT_IS_TOP_LEVEL)
    # make our version known to projects which add us as a subdirectory
    set(alpaca-core-sdk_VERSION ${alpaca-core-sdk_VERSION} PARENT_SCOPE)
endif()

option(AC_BUILD_TESTS "${PROJECT_NAME}: build tests" ${testsDefault})
option(AC_BUILD_EXAMPLES "${PROJECT_NAME}: build examples" ${examplesDefault})
mark_as_advanced(AC_BUILD_TESTS AC_BUILD_EXAMPLES)

#################
# global packages

if(AC_BUILD_TESTS)
    ac_dep(doctest-util)
endif()

#################
# subdirs/targets

# we thought about allowing finer-grain config from here
# motivating examples would be:
# - build only astl and jalog
# - build only frame
# - don't build dummy
# we could do it if there's interest

add_subdirectory(common)

add_subdirectory(frame)
add_subdirectory(local)

#################
# export and install
configure_file(
    alpaca-core-sdk-config-version.in.cmake
    alpaca-core-sdk-config-version.cmake
    @ONLY
)

# export
export(EXPORT ac-common-targets)
export(EXPORT ac-frame-targets)
export(EXPORT ac-local-targets)
configure_file(
    alpaca-core-sdk-config-export.in.cmake
    alpaca-core-sdk-config.cmake
    @ONLY
)

# install
install(EXPORT ac-common-targets
    DESTINATION share/cmake
)
install(EXPORT ac-frame-targets
    DESTINATION share/cmake
)
install(EXPORT ac-local-targets
    DESTINATION share/cmake
)
