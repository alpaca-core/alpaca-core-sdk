# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(magic_enum)

add_library(ac-schema INTERFACE)
add_library(ac::schema ALIAS ac-schema)
target_link_libraries(ac-schema INTERFACE
    ac::dict
    magic_enum::magic_enum
)
target_include_directories(ac-schema INTERFACE code)
ac_add_test_subdir()
