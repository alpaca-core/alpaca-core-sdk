# Copyright (c) Borislav Stanimirov
# SPDX-License-Identifier: MIT
#

icm_add_test(
    NAME ac-jalog-qwrite
    TARGET test-ac-jalog-qwrite
    LIBRARIES
        ac::astl
        doctest::main
    SOURCES t-qwrite.cpp
)

macro(jalog_test test)
    add_doctest_lib_test(${test} ac-jalog ${ARGN})
endmacro()

jalog_test(core t-core.cpp)
jalog_test(defaults t-defaults.cpp)
