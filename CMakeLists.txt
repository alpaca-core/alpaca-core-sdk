# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(ac-local
    VERSION 0.1.4
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

include(ac_local_lib)
include(ac_local_plugin_util)

#################
# cfg
if(NOT PROJECT_IS_TOP_LEVEL)
    # make our version known to projects which add us as a subdirectory
    set(ac-local_VERSION ${ac-local_VERSION} PARENT_SCOPE)
endif()

option(AC_LOCAL_BUILD_TESTS "${PROJECT_NAME}: build tests" ${testsDefault})
option(AC_LOCAL_BUILD_EXAMPLES "${PROJECT_NAME}: build examples" ${examplesDefault})
mark_as_advanced(AC_LOCAL_BUILD_TESTS AC_LOCAL_BUILD_EXAMPLES)

#################
# global packages

if(AC_LOCAL_BUILD_TESTS)
    CPMAddPackage(gh:iboB/doctest-util@0.1.3)
    set_target_properties(doctest PROPERTIES FOLDER test)
    set_target_properties(doctest-main PROPERTIES FOLDER test)
    enable_testing()
endif()

macro(ac_local_add_test_subdir)
    if(AC_LOCAL_BUILD_TESTS)
        add_subdirectory(test)
    endif()
endmacro()

#################
# subdirs/targets

# we thought about allowing finer-grain config from here
# motivating examples would be:
# - build only astl and jalog
# - build only dict
# - don't build dummy
# we could do it if there's interest

add_subdirectory(astl)
add_subdirectory(ac-test-util)

add_subdirectory(jalog)

add_subdirectory(frame)
add_subdirectory(local)
add_subdirectory(schema)

add_subdirectory(dummy-plugin)

#################
# export and install
configure_file(
    ac-local-config-version.in.cmake
    ac-local-config-version.cmake
    @ONLY
)

# export
export(EXPORT ac-frame-targets)
export(EXPORT ac-local-targets)
configure_file(
    ac-local-config-export.in.cmake
    ac-local-config.cmake
    @ONLY
)

# install
install(EXPORT ac-frame-targets
    DESTINATION share/cmake
)
install(EXPORT ac-local-targets
    DESTINATION share/cmake
)
