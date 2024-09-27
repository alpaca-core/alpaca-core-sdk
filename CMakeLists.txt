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
include(CMakeDependentOption)
include(ac_dep)
include(ac_auto_option)

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

# normally we wouldn't just prevent a warning globaly like the following lines,
# but it is our opinion that this warning is just stupid
if(NOT MSVC)
	add_compile_options(-Wno-missing-field-initializers)
endif()

option(BUILD_SHARED_LIBS "${CMAKE_PROJECT_NAME}: build shared libraries" ON)

option(AC_BUILD_LOCAL "${CMAKE_PROJECT_NAME}: build local inference api" ON)

set(AC_INFERENCE_LIBS "all" CACHE STRING "${CMAKE_PROJECT_NAME}: inference libraries to build")

option(AC_BUILD_TESTS "${CMAKE_PROJECT_NAME}: build tests" ${alpacaCoreIsRoot})
option(AC_BUILD_EXAMPLES "${CMAKE_PROJECT_NAME}: build examples" ${alpacaCoreIsRoot})
option(AC_BUILD_POC "${CMAKE_PROJECT_NAME}: build proof of concept targets" ${alpacaCoreIsRoot})
option(AC_BUILD_DOCS "${CMAKE_PROJECT_NAME}: build documentation" ${alpacaCoreIsRoot})
mark_as_advanced(AC_BUILD_TESTS AC_BUILD_EXAMPLES AC_BUILD_POC AC_BUILD_DOCS)

#################
# global packages

if(AC_BUILD_DOCS)
    # Find Doxygen
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(doxygenOutDir ${PROJECT_SOURCE_DIR}/out)

        # Set input and output files
        set(DOXYGEN_IN ${PROJECT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYGEN_OUT ${doxygenOutDir}/Doxyfile)
        set(DOXYGEN_OUTPUT_DIR ${doxygenOutDir}/doc-doxygen)

        # Define input directories
        set(DOC_INPUT_DIRS "${PROJECT_SOURCE_DIR}/api")

        # Request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

        # Pass PROJECT_SOURCE_DIR to Doxygen
        set(PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR})

        # Create custom command
        add_custom_command(
            OUTPUT ${DOXYGEN_OUTPUT_DIR}
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
            DEPENDS ${DOC_INPUT_DIRS}
        )

        # Create target that depends on the custom command
        add_custom_target(ac-generate-docs
            DEPENDS ${DOXYGEN_OUTPUT_DIR}
        )
    else()
        message(WARNING "Doxygen needs to be installed to generate the documentation")
    endif()
endif()

CPMAddPackage(gh:iboB/splat@1.3.3)
CPMAddPackage(gh:iboB/itlib@1.11.4)
CPMAddPackage(gh:iboB/jalog@0.4.3)

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
    add_subdirectory(inference)
endif()

if(AC_BUILD_LOCAL)
    # add after inference libs so that elements from them can be wrapped as well
    add_subdirectory(wrapper)
endif()

if(AC_BUILD_POC)
    add_subdirectory(poc)
endif()
