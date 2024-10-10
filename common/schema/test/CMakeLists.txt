# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(schema_test test)
    add_doctest_lib_test(${test} ac-schema t-${test}.cpp)
endmacro()

schema_test(SchemaItem)
schema_test(SchemaEnum)
