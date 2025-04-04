# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(add_xec_test test)
    add_doctest_lib_test(${test} ac-xec t-${test}.cpp ${ARGN})
endmacro()

add_xec_test(coro)
add_xec_test(generator)
add_xec_test(thread_runner)
add_xec_test(coro-mt)

ac_dep(b_stacktrace)
add_xec_test(coro-stack LIBRARIES b_stacktrace::b_stacktrace)
