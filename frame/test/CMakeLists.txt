# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(add_frame_test test)
    add_doctest_lib_test(${test} ac-frame t-${test}.cpp)
endmacro()

add_frame_test(Dict)

add_frame_test(Stream)

add_frame_test(Field)

add_frame_test(op-traits)
add_frame_test(schema-visitors)
