# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(llama_test test)
    add_doctest_lib_test(${test} ac-llama t-${test}.cpp)
endmacro()

llama_test(ChatTemplate)
