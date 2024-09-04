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

option(AC_BUILD_API "${CMAKE_PROJECT_NAME}: build API" ON)
cmake_dependent_option(AC_BUILD_LOCAL_PROVIDER "${CMAKE_PROJECT_NAME}: build local inference provider" ON
    AC_BUILD_API OFF)

option(AC_BUILD_TESTS "${CMAKE_PROJECT_NAME}: build tests" ${alpacaCoreIsRoot})
option(AC_BUILD_EXAMPLES "${CMAKE_PROJECT_NAME}: build examples" ${alpacaCoreIsRoot})
option(AC_BUILD_POC "${CMAKE_PROJECT_NAME}: build proof of concept targets" ${alpacaCoreIsRoot})
option(AC_BUILD_DOCS "${CMAKE_PROJECT_NAME}: build documentation" ${alpacaCoreIsRoot})
mark_as_advanced(AC_BUILD_TESTS AC_BUILD_EXAMPLES AC_BUILD_POC AC_BUILD_DOCS)

#################
# global packages

# Find Doxygen
if(AC_BUILD_DOCS)
    # Find Doxygen
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        # Set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doc_doxygen)

        # Define input directories
        set(DOC_INPUT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/api")

        # Request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

        # Create custom command
        add_custom_command(
            OUTPUT ${DOXYGEN_OUTPUT_DIR}
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
            DEPENDS ${DOC_INPUT_DIRS}
        )

        # Create target that depends on the custom command
        add_custom_target(generate-docs
            DEPENDS ${DOXYGEN_OUTPUT_DIR}
        )
    else()
        message(WARNING "Doxygen needs to be installed to generate the documentation")
    endif()
endif()

CPMAddPackage(gh:iboB/splat@1.3.3)
CPMAddPackage(gh:iboB/itlib@1.11.3)
CPMAddPackage(gh:iboB/jalog@0.4.3)

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

CPMAddPackage(
    NAME ac-test-data-whisper
    VERSION 1.0.0
    GIT_REPOSITORY https://huggingface.co/alpaca-core/ac-test-data-whisper
    GIT_TAG f33d981742198f2b55494265fc9d43156b39a30d
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
