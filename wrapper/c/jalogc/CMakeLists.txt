# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(jalogc JALOGC
    code/jalogc.h
    code/jalogc.cpp
)
target_include_directories(jalogc INTERFACE code)
target_link_libraries(jalogc
    PUBLIC
        splat::splat
    PRIVATE
        ac::jalog
)
add_library(ac-c::jalogc ALIAS jalogc)

ac_add_test_subdir()
