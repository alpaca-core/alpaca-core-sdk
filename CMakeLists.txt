# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(ac-local
    VERSION 0.1.0
    DESCRIPTION "Alpaca Core Local SDK"
    LANGUAGES C CXX
)

set(PROJECT_VERSION_TAG dev)

#################
# cpm
if(NOT CPM_SOURCE_CACHE AND NOT DEFINED ENV{CPM_SOURCE_CACHE})
    set(CPM_SOURCE_CACHE "${CMAKE_CURRENT_SOURCE_DIR}/.cpm")
    message(STATUS "Setting cpm cache dir to: ${CPM_SOURCE_CACHE}")
endif()
include(./get_cpm.cmake)

#################
# cmake lib
CPMAddPackage(gh:iboB/icm@1.5.4)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
    "${icm_SOURCE_DIR}"
)

include(icm_add_lib)
include(ac_lib_subdirs)
include(icm_testing)
include(icm_build_failure_testing)
include(CMakeDependentOption)
include(ac_dep)
include(icm_option)
include(ac_module)

#################
# cfg
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(alpacaCoreIsRoot YES)
else()
    set(alpacaCoreIsRoot NO)
endif()

if(alpacaCoreIsRoot)
    # setup various compiler settings that would otherwise be expected to come
    # from the outside
    include(./root-setup.cmake)
endif()

# normally we wouldn't just prevent a warning globaly like the following lines,
# but it is our opinion that this warning is just stupid
if(NOT MSVC)
	add_compile_options(-Wno-missing-field-initializers)
endif()

option(BUILD_SHARED_LIBS "${CMAKE_PROJECT_NAME}: build shared libraries" ON)

option(AC_BUILD_LOCAL "${CMAKE_PROJECT_NAME}: build local inference api" ON)

set(AC_INFERENCE_LIBS "all" CACHE STRING "${CMAKE_PROJECT_NAME}: inference libraries to build")

cmake_dependent_option(AC_BUILD_ACORD "${CMAKE_PROJECT_NAME}: build Alpaca Core Daemon (acord)" ${alpacaCoreIsRoot}
    AC_BUILD_LOCAL OFF
)

option(AC_BUILD_TOOLS "${CMAKE_PROJECT_NAME}: build tools" ${alpacaCoreIsRoot})

option(AC_BUILD_TESTS "${CMAKE_PROJECT_NAME}: build tests" ${alpacaCoreIsRoot})
option(AC_BUILD_EXAMPLES "${CMAKE_PROJECT_NAME}: build examples" ${alpacaCoreIsRoot})
option(AC_BUILD_POC "${CMAKE_PROJECT_NAME}: build proof of concept targets" ${alpacaCoreIsRoot})
mark_as_advanced(AC_BUILD_TESTS AC_BUILD_EXAMPLES AC_BUILD_POC)

#################
# global packages

CPMAddPackage(gh:iboB/itlib@1.11.4)

if(AC_BUILD_TESTS)
    CPMAddPackage(gh:iboB/doctest-util@0.1.2)
    set_target_properties(doctest PROPERTIES FOLDER test)
    set_target_properties(doctest-main PROPERTIES FOLDER test)
    enable_testing()
endif()

#################
# subdirs/targets

add_subdirectory(common)
add_subdirectory(dict)

ac_add_local_subdir()

if(NOT AC_INFERENCE_LIBS STREQUAL "none")
    #add_subdirectory(inference)
endif()

if(AC_BUILD_ACORD)
    add_subdirectory(acord)
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
set(namespace ac::)

# export
export(EXPORT ac-local-targets
    NAMESPACE ${namespace}
)
configure_file(
    cmake/lib/plugin_util.cmake
    plugin_util.cmake
    COPYONLY
)
configure_file(
    ac-local-config-export.in.cmake
    ac-local-config.cmake
    @ONLY
)

# install
install(EXPORT ac-local-targets
    NAMESPACE ${namespace}
    DESTINATION share/cmake
)
