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
        jalog::jalog
        jalog::sinklib
)
add_library(ac-c::jalogc ALIAS jalogc)

ac_add_test_subdir()
