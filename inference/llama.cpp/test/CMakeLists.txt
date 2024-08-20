# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(llama_test test)
    add_doctest_lib_test(${test} ac-llama t-${test}.cpp)
endmacro()

llama_test(Antiprompt)
llama_test(ChatFormat)
llama_test(Session)

add_doctest_lib_test(integration ac-llama
    SOURCES
        t-integration.cpp
    LIBRARIES
        ac-test-data::llama
)
