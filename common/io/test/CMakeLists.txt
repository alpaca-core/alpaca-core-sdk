# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(add_io_test test)
    add_doctest_lib_test(${test} ac-io t-${test}.cpp)
endmacro()

add_io_test(status)
add_io_test(buffered_channel)
add_io_test(channel_stream)
add_io_test(xio)
add_io_test(blocking_io)
add_io_test(sync-io)
