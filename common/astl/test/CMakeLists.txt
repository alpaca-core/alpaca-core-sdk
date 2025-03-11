# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(astl_test test)
    add_doctest_lib_test(${test} astl t-${test}.cpp)
endmacro()

astl_test(iile)
astl_test(mem_ext)
astl_test(move)
astl_test(move_capture)
astl_test(safe_func)
astl_test(throw_ex)
astl_test(u8c)
astl_test(tuple_util)
astl_test(version)
astl_test(sc_queue)
astl_test(timeout)
astl_test(eager_coro_helper)

icm_add_multiple_build_failure_tests(
    SOURCES
        t-move-bf.cpp
    LIBRARIES astl
    PREFIX astl
    FOLDER bf-test
)

add_subdirectory(workarounds)
