# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_library(ac-c-test-util INTERFACE)
add_library(ac-c-dev::test-util ALIAS ac-c-test-util)
target_include_directories(ac-c-test-util INTERFACE include)

function(add_unity_lib_test test lib)
    cmake_parse_arguments(ARG "" "" "SOURCES;LIBRARIES;LABELS" ${ARGN})
    icm_add_test(
        NAME ${lib}-${test}
        TARGET test-${lib}-${test}
        LIBRARIES
            unity::framework
            ac-c-dev::test-util
            ${lib}
            ${ARG_LIBRARIES}
        SOURCES
            ${ARG_UNPARSED_ARGUMENTS}
            ${ARG_SOURCES}
        LABELS ${ARG_LABELS}
        FOLDER test
    )
endfunction()

add_subdirectory(doctest-unity)
